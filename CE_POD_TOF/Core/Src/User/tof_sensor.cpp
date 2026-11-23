/*
 * tof_sensor.c
 *
 *  Created on: Dec 26, 2024
 *      Author: root
 */



#ifdef __cplusplus
#include "../../Core/Inc/User/tof_sensor.hpp"
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/



/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* uncomment following to use directly the bare driver instead of the BSP */
#define USE_VL53L5CX_BARE_DRIVER
#define VL_TIMING_BUDGET (16U) /* 5 ms < TimingBudget < 100 ms */
// #define VL_TIMING_BUDGET (30U) /* 5 ms < TimingBudget < 100 ms */
#define VL_RANGING_FREQUENCY (60U) /* Ranging frequency Hz (shall be consistent with TimingBudget value) */
// #define VL_RANGING_FREQUENCY (10U) /* Ranging frequency Hz (shall be consistent with TimingBudget value) */
#define VL53L5CX_POLLING_PERIOD (4)
// #define VL53L5CX_POLLING_PERIOD (10)




/* Private variables ---------------------------------------------------------*/
#ifndef USE_VL53L5CX_BARE_DRIVER
static RANGING_SENSOR_Capabilities_t VL53L5CX_Cap;
#endif
static RANGING_SENSOR_ProfileConfig_t RS_Profile;
static RANGING_SENSOR_Result_t RS_Result;

static int32_t rs_status = 0;
volatile uint8_t TOF_EventDetected = 0;
// static VL53L5CX_Object_t *pL5obj[CUSTOM_RANGING_INSTANCES_NBR];




/* Private function prototypes -----------------------------------------------*/
#ifdef USE_VL53L5CX_BARE_DRIVER
static uint8_t map_target_status_usr(uint8_t status);
static int32_t convert_data_format_usr(VL53L5CX_Object_t *pObj,
                                   VL53L5CX_ResultsData *data, RANGING_SENSOR_Result_t *pResult);
#endif
static auto VL53L5CX_SimpleRanging_Init() -> void;





static void VL53L5CX_SimpleRanging_Process(uint32_t instance);
static void print_result_measurements(RANGING_SENSOR_Result_t *Result);
static void toggle_resolution_sensor(void);
static void toggle_signal_and_ambient_sensor(void);
static void clear_result_screen(void);
static void display_cmd_banner(void);
static void handle_command(uint8_t cmd);

static uint8_t get_user_key(void);
static uint32_t com_uart_has_data(void);

void VL53L5CX_TOF_Init(void)
{
    /* USER CODE BEGIN SV */

    /* USER CODE END SV */

    /* USER CODE BEGIN TOF_Init_PreTreatment */

    /* USER CODE END TOF_Init_PreTreatment */

    /* Initialize the peripherals and the TOF components */
    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::DEBUG_LEVEL) {
        printf("[DEBUG] VL53L5CX_TOF_Init Before\n\r\n\r");
    }
    VL53L5CX_SimpleRanging_Init();

    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::DEBUG_LEVEL) {
        printf("[DEBUG] VL53L5CX_TOF_Init After\n\r\n\r");
    }
    /* USER CODE BEGIN TOF_Init_PostTreatment */

    /* USER CODE END TOF_Init_PostTreatment */
}

/*
 * LM background task
 */
void VL53L5CX_TOF_Process(uint32_t instance)
{
    /* USER CODE BEGIN TOF_Process_PreTreatment */

    /* USER CODE END TOF_Process_PreTreatment */

    VL53L5CX_SimpleRanging_Process(instance);

    /* USER CODE BEGIN TOF_Process_PostTreatment */

    /* USER CODE END TOF_Process_PostTreatment */
}

// Vorsichtig mit noreturn (wird von clang-tidy empfohlen) diese führt aber hier dazu,
// dass main-loop nur ein einziges Mal ausgeführt wird
//[[noreturn]] static void VL53L5CX_SimpleRanging_Init(void)
static void VL53L5CX_SimpleRanging_Init(void)
{
    // Initialize Virtual COM Port
    // BSP_COM_Init(COM1);

    // printf("\033[2H\033[2J");
    printf("Sensor initialization...\n\r\n\r");
    constexpr uint16_t actual_address = 0x52;
    constexpr uint16_t new_address_1 = 0x13;
    uint16_t address_to_print = 0;
    // constexpr uint16_t new_address_2 = 0x39;




    // Diese folgende Reihenfolge der Implementierung führt dazu, dass new_address_2 zu 0x0000 wird.
    // constexpr uint16_t new_address_2 = 0x39;
    // uint16_t address_to_print = 0;




    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
        printf("[INFO] Start initialization and Set new Adresse for Sensor:\t %d\n\r\n\r", CUSTOM_VL53L5CX_1 + 1);
    }
    /// Nur am Anfang, um die Adresse zurückzusetzen zu 0x52, danach darf diese nicht mehr
    /// ausgeführt werden, da diese, den I2C-Bus zurück setzt, was dazu führt, dass die
    /// gesetzte Adresse nicht mehr gultig ist, das diese zur 0x52 zurückgesetz wird
    /// Phase 1: Strom für beiden Sensoren einschalten und deren Adressen zurücksetzen,
    /// falls diese bei eine vorherige Prozesse gesetzt wurde.

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_PWR_EN_PORT, CUSTOM_VL53L5CX_1_PWR_EN_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_PWR_EN_PORT, CUSTOM_VL53L5CX_1_PWR_EN_PIN, GPIO_PIN_SET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_PWR_EN_PORT, CUSTOM_VL53L5CX_2_PWR_EN_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_PWR_EN_PORT, CUSTOM_VL53L5CX_2_PWR_EN_PIN, GPIO_PIN_SET);
    HAL_Delay(2);


    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_PWR_EN_PORT, CUSTOM_VL53L5CX_3_PWR_EN_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_PWR_EN_PORT, CUSTOM_VL53L5CX_3_PWR_EN_PIN, GPIO_PIN_SET);
    HAL_Delay(2);


    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_PWR_EN_PORT, CUSTOM_VL53L5CX_4_PWR_EN_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_PWR_EN_PORT, CUSTOM_VL53L5CX_4_PWR_EN_PIN, GPIO_PIN_SET);
    HAL_Delay(2);










    /// Phase 1 Beide Sensoren desiablen um die Adressen der einzelnen Sensoren zu ändern

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_LPn_PORT, CUSTOM_VL53L5CX_1_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_LPn_PORT, CUSTOM_VL53L5CX_2_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_LPn_PORT, CUSTOM_VL53L5CX_3_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);


    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_LPn_PORT, CUSTOM_VL53L5CX_4_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    /// Phase 2: Sensor 1 mit der seine aktuellen Adresse 0x52 initialiseren

    // Sensor 1

    is_initialized[1] = VL53L5CX_SimpleRanging_Secure_Init(CUSTOM_VL53L5CX_1, actual_address);



    /// Phase 4: Set Sensor Adress
    if (is_initialized[1] == true) {
        Secure_Set_Sensor_Address(CUSTOM_VL53L5CX_1, new_address_1);

        /// [INFO] Überprüfen, ob die neue Adresse new_adress gesetzt wird.
        rs_status = TOF_RANGING_SENSOR_GetAddress(CUSTOM_VL53L5CX_1, &address_to_print);
        if (rs_status != BSP_ERROR_NONE)
        {
            if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
                printf("[ERROR] Get Sensor %d Address failed\n\r", CUSTOM_VL53L5CX_1 + 1);
            }

            // while (true);
        }
        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d new Adresse :0x%04X\t\n\r",CUSTOM_VL53L5CX_1 + 1, address_to_print);
        }

        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d Konfiguration: Erfolgreich \n\r\n\r", CUSTOM_VL53L5CX_1 + 1);
        }
    }



    /// Konfiguration Sensor 2
    ///     1. Deint senor 1
    ///     2. Reset LPN_1 Sesnor 1
    ///     2. Set LPN_2 Sesnor 2

    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
        printf("[INFO] Start initialization and Set new Adresse for Sensor:\t %d\n\r\n\r", CUSTOM_VL53L5CX_2 + 1);
    }


    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_LPn_PORT, CUSTOM_VL53L5CX_1_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_LPn_PORT, CUSTOM_VL53L5CX_3_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_LPn_PORT, CUSTOM_VL53L5CX_4_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    is_initialized[2] = VL53L5CX_SimpleRanging_Secure_Init(CUSTOM_VL53L5CX_2, actual_address);


    /// Phase 4: Set Sensor Adress
    if (is_initialized[2] == true) {
        constexpr uint16_t new_address_2 = 0x14;
        Secure_Set_Sensor_Address(CUSTOM_VL53L5CX_2, new_address_2);

        /// [INFO] Überprüfen, ob die neue Adresse new_adress gesetzt wird.
        rs_status = TOF_RANGING_SENSOR_GetAddress(CUSTOM_VL53L5CX_2, &address_to_print);
        if (rs_status != BSP_ERROR_NONE)
        {
            if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
                printf("[ERROR] Get Sensor %d Address failed\n\r", CUSTOM_VL53L5CX_2 + 1);
            }

            // while (true);
        }
        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d new Adresse :\t 0x%04X\n\r",CUSTOM_VL53L5CX_2 + 1, address_to_print);
        }


        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d Konfiguration: Erfolgreich \n\r\n\r", CUSTOM_VL53L5CX_2 + 1);
        }

        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d Konfiguration: Erfolgreich \n\r\n\r", CUSTOM_VL53L5CX_2 + 1);
        }
    }

    /// Konfiguration Sensor 3





    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
        printf("[INFO] Start initialization and Set new Adresse for Sensor:\t %d\n\r\n\r", CUSTOM_VL53L5CX_3 + 1);
    }



    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_LPn_PORT, CUSTOM_VL53L5CX_1_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_LPn_PORT, CUSTOM_VL53L5CX_2_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_LPn_PORT, CUSTOM_VL53L5CX_4_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);


    constexpr uint16_t new_address_3 = 0x15;
    // VL53L5CX_SimpleRanging_Secure_Init(CUSTOM_VL53L5CX_3, new_address_3);

    is_initialized[3] = VL53L5CX_SimpleRanging_Secure_Init(CUSTOM_VL53L5CX_3, actual_address);

    if (is_initialized[3] == true) {
        Secure_Set_Sensor_Address(CUSTOM_VL53L5CX_3, new_address_3);

        /// [INFO] Überprüfen, ob die neue Adresse new_adress gesetzt wird.
        rs_status = TOF_RANGING_SENSOR_GetAddress(CUSTOM_VL53L5CX_3, &address_to_print);
        if (rs_status != BSP_ERROR_NONE)
        {
            if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
                printf("[ERROR] Get Sensor %d Address failed\n\r", CUSTOM_VL53L5CX_3 + 1);
            }
            // while (true);
        }
        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d new Adresse :\t 0x%04X\n\r", CUSTOM_VL53L5CX_3 + 1, address_to_print);
        }


        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d Konfiguration: Erfolgreich \n\r\n\r", CUSTOM_VL53L5CX_3 + 1);
        }

        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d Konfiguration: Erfolgreich \n\r\n\r", CUSTOM_VL53L5CX_3 + 1);
        }
    }





    /// Konfiguration Sensor 4

    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
        printf("[INFO] Start initialization and Set new Adresse for Sensor:\t %d\n\r\n\r", CUSTOM_VL53L5CX_4 + 1);
    }


    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_LPn_PORT, CUSTOM_VL53L5CX_1_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);


    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_LPn_PORT, CUSTOM_VL53L5CX_2_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_LPn_PORT, CUSTOM_VL53L5CX_3_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    constexpr uint16_t new_address_4 = 0x16;
    // VL53L5CX_SimpleRanging_Secure_Init(CUSTOM_VL53L5CX_4, new_address_4);
    is_initialized[4] = VL53L5CX_SimpleRanging_Secure_Init(CUSTOM_VL53L5CX_4, actual_address);

    if (is_initialized[4] == true) {
        Secure_Set_Sensor_Address(CUSTOM_VL53L5CX_4, new_address_4);

        /// [INFO] Überprüfen, ob die neue Adresse new_adress gesetzt wird.
        rs_status = TOF_RANGING_SENSOR_GetAddress(CUSTOM_VL53L5CX_4, &address_to_print);
        if (rs_status != BSP_ERROR_NONE)
        {
            if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
                printf("[ERROR] Get Sensor %d Address failed\n\r", CUSTOM_VL53L5CX_4 + 1);
            }
            // while (true);
        }

        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d new Adresse :\t 0x%04X\n\r",CUSTOM_VL53L5CX_4 + 1, address_to_print);
        }

        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d Konfiguration: Erfolgreich \n\r\n\r", CUSTOM_VL53L5CX_4 + 1);
        }

        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
            printf("[INFO] Sensor %d Konfiguration: Erfolgreich \n\r\n\r", CUSTOM_VL53L5CX_4 + 1);
        }
    }




    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_LPn_PORT, CUSTOM_VL53L5CX_1_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_LPn_PORT, CUSTOM_VL53L5CX_1_LPn_PIN, GPIO_PIN_SET);
    HAL_Delay(2);


    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_LPn_PORT, CUSTOM_VL53L5CX_2_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_LPn_PORT, CUSTOM_VL53L5CX_2_LPn_PIN, GPIO_PIN_SET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_LPn_PORT, CUSTOM_VL53L5CX_3_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_LPn_PORT, CUSTOM_VL53L5CX_3_LPn_PIN, GPIO_PIN_SET);
    HAL_Delay(2);


    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_LPn_PORT, CUSTOM_VL53L5CX_4_LPn_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_LPn_PORT, CUSTOM_VL53L5CX_4_LPn_PIN, GPIO_PIN_SET);
    HAL_Delay(2);
}



void VL53L5CX_TOF_Prepare(uint32_t instance) {
        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::DEBUG_LEVEL) {
        printf("[DEBUG] vl53l5cx_start_ranging start Sensor %d\n\r", instance + 1);
    }
    // auto *pL5obj = static_cast<VL53L5CX_Object_t *>(TOF_RANGING_CompObj[instance]);

    // VL53L5CX_Object_t *pL5objj = static_cast<VL53L5CX_Object_t *>(TOF_RANGING_CompObj[instance]);
    pL5obj[instance] = static_cast<VL53L5CX_Object_t *>(TOF_RANGING_CompObj[instance]);


    //RS_Profile.RangingProfile = RS_PROFILE_4x4_CONTINUOUS;
    RS_Profile.RangingProfile = RS_PROFILE_8x8_CONTINUOUS;
    // RS_Profile.TimingBudget = VL_TIMING_BUDGET;
    RS_Profile.Frequency = VL_RANGING_FREQUENCY; /* Ranging frequency Hz (shall be consistent with TimingBudget value) */
    RS_Profile.EnableAmbient = 0; /* Enable: 1, Disable: 0 */
    RS_Profile.EnableSignal = 0; /* Enable: 1, Disable: 0 */

    pL5obj[instance]->IsAmbientEnabled = RS_Profile.EnableAmbient;
    pL5obj[instance]->IsSignalEnabled = RS_Profile.EnableSignal;

    /*
       use case VL53L5CX_PROFILE_4x4_CONTINUOUS:
    */
    //rs_status = vl53l5cx_set_resolution(&(pL5obj->Dev), VL53L5CX_RESOLUTION_4X4);
    rs_status = vl53l5cx_set_resolution(&(pL5obj[instance]->Dev), VL53L5CX_RESOLUTION_8X8);
    rs_status |= vl53l5cx_set_ranging_mode(&(pL5obj[instance]->Dev), VL53L5CX_RANGING_MODE_CONTINUOUS);
    // rs_status |= vl53l5cx_set_integration_time_ms(&(pL5obj->Dev), VL_TIMING_BUDGET);
    rs_status |= vl53l5cx_set_ranging_frequency_hz(&(pL5obj[instance]->Dev), VL_RANGING_FREQUENCY);

    if (rs_status != VL53L5CX_STATUS_OK)
    {
        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
            printf("[ERROR] Configuration programming error!\n\n\r");
        }
        while (1);
    }

    rs_status = vl53l5cx_start_ranging(&(pL5obj[instance]->Dev));
    if (rs_status != VL53L5CX_STATUS_OK)
    {
        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
            printf("[ERROR] vl53l5cx_start_ranging failed\n\r");
        }
        while (1);
    }


    uint32_t adress = 0;

    VL53L5CX_GetAddress(pL5obj[instance], &adress);
    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::DEBUG_LEVEL) {
        printf("[DEBUG] Sensor %d Adresse:\t0x%04X\n\n\r", instance + 1, adress);
    }
}
void prepare_tof_sensor() {
    if (is_initialized[1] == true) {
        VL53L5CX_TOF_Prepare(CUSTOM_VL53L5CX_1);
    }
    if (is_initialized[2] == true) {
        VL53L5CX_TOF_Prepare(CUSTOM_VL53L5CX_2);
    }
    if (is_initialized[3] == true) {
        VL53L5CX_TOF_Prepare(CUSTOM_VL53L5CX_3);
    }
    if (is_initialized[4] == true) {
        VL53L5CX_TOF_Prepare(CUSTOM_VL53L5CX_4);
    }
}





static void VL53L5CX_SimpleRanging_Process_time(uint32_t instance) {

    uint32_t start = HAL_GetTick(); // Startzeit in Millisekunden
    uint32_t x = 0;
    uint8_t NewDataReady = 0;
    static VL53L5CX_ResultsData data;

    while (x<1000)
    {
        /* polling mode */
        (void)vl53l5cx_check_data_ready(&(pL5obj[instance]->Dev), &NewDataReady);
        /// Nur für Print kann auskomentiert werden
        if (NewDataReady != 0)
        {
            rs_status = vl53l5cx_get_ranging_data(&(pL5obj[instance]->Dev), &data);
            // printf("%p, \n\r", data.distance_mm);
            if (rs_status == VL53L5CX_STATUS_OK)
            {
                /*
                 Convert the data format to Result format.
                 Note that you can print directly from data format
                */

                //if (convert_data_format_usr(pL5obj[instance], &data, &RS_Result) < 0)
                //{
                //    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
                //        printf("[ERROR] convert_data_format failed\n\r");
                //    }
                //    while (1);
                //}

                // print_result_measurements(&RS_Result);
            }

            x++;

        }

        //if (com_uart_has_data())
        //{
        //    handle_command(get_user_key());
        //}

        HAL_Delay(VL53L5CX_POLLING_PERIOD);
        // x++;
    }

    /* USER CODE BEGIN TOF_TimeTracking*/
    uint32_t end = HAL_GetTick(); // Endzeit in Millisekunden

    uint32_t duration_ms = end - start; // Zeit in Millisekunden
    [[maybe_unused]] uint32_t pId = 0;

    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
        printf("[INFO] Funktion dauerte: %lu ms\n\r", duration_ms);
    }
    HAL_Delay(4000);

    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::DEBUG_LEVEL) {
        printf("[DEBUG] vl53l5cx_start_ranging end\n\r");
    }

    /* USER CODE END TOF_TimeTracking */
}



static void VL53L5CX_SimpleRanging_Process(uint32_t instance) {
    uint32_t x = 0;
    uint8_t NewDataReady = 0;
    static VL53L5CX_ResultsData data;

    while (x<1000)
    {
        /* polling mode */
        (void)vl53l5cx_check_data_ready(&(pL5obj[instance]->Dev), &NewDataReady);
        /// Nur für Print kann auskomentiert werden
        if (NewDataReady != 0)
        {
            rs_status = vl53l5cx_get_ranging_data(&(pL5obj[instance]->Dev), &data);
            if (rs_status == VL53L5CX_STATUS_OK)
            {
                /*
                 Convert the data format to Result format.
                 Note that you can print directly from data format
                */

                if (convert_data_format_usr(pL5obj[instance], &data, &RS_Result) < 0)
                {
                    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
                        printf("[ERROR] convert_data_format failed\n\r");
                    }
                    while (1);
                }

            }

            x++;

        }

        HAL_Delay(VL53L5CX_POLLING_PERIOD);
    }
    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::DEBUG_LEVEL) {
        printf("[DEBUG] vl53l5cx_start_ranging end\n\r");
    }

    /* USER CODE END TOF_TimeTracking */
}



static void print_result_measurements(RANGING_SENSOR_Result_t *Result)
{
    int8_t i;
    int8_t j;
    int8_t k;
    int8_t l;

    const uint8_t zones_per_line = ((RS_Profile.RangingProfile == RS_PROFILE_8x8_AUTONOMOUS) ||
                              (RS_Profile.RangingProfile == RS_PROFILE_8x8_CONTINUOUS))
                                 ? 8
                                 : 4;

    display_cmd_banner();

    printf("Cell Format :\n\n\r");
    for (l = 0; l < RANGING_SENSOR_NB_TARGET_PER_ZONE; l++)
    {
        printf(" \033[38;5;10m%20s\033[0m : %20s\n\r", "Distance [mm]", "Status");
        if ((RS_Profile.EnableAmbient != 0) || (RS_Profile.EnableSignal != 0))
        {
            printf(" %20s : %20s\n\r", "Signal [kcps/spad]", "Ambient [kcps/spad]");
        }
    }

    printf("\n\n\r");

    for (j = 0; j < Result->NumberOfZones; j += zones_per_line)
    {
        for (i = 0; i < zones_per_line; i++) /* number of zones per line */
        {
            printf(" -----------------");
        }
        printf("\n\r");

        for (i = 0; i < zones_per_line; i++)
        {
            printf("|                 ");
        }
        printf("|\n\r");

        for (l = 0; l < RANGING_SENSOR_NB_TARGET_PER_ZONE; l++)
        {
            /* Print distance and status */
            for (k = (zones_per_line - 1); k >= 0; k--)
            {
                if (Result->ZoneResult[j + k].NumberOfTargets > 0)
                {
                    if ((long)Result->ZoneResult[j + k].Distance[l] < 500)
                    {
                        printf("| \033[38;5;9m%5ld\033[0m  :  %5ld ",
                               (long)Result->ZoneResult[j + k].Distance[l],
                               (long)Result->ZoneResult[j + k].Status[l]);
                    }
                    else
                    {
                        printf("| \033[38;5;10m%5ld\033[0m  :  %5ld ",
                               (long)Result->ZoneResult[j + k].Distance[l],
                               (long)Result->ZoneResult[j + k].Status[l]);
                    }
                }
                else
                    printf("| %5s  :  %5s ", "X", "X");
            }
            printf("|\n\r");

            if ((RS_Profile.EnableAmbient != 0) || (RS_Profile.EnableSignal != 0))
            {
                /* Print Signal and Ambient */
                for (k = (zones_per_line - 1); k >= 0; k--)
                {
                    if (Result->ZoneResult[j + k].NumberOfTargets > 0)
                    {
                        if (RS_Profile.EnableSignal != 0)
                        {
                            printf("| %5ld  :  ", (long)Result->ZoneResult[j + k].Signal[l]);
                        }
                        else
                            printf("| %5s  :  ", "X");

                        if (RS_Profile.EnableAmbient != 0)
                        {
                            printf("%5ld ", (long)Result->ZoneResult[j + k].Ambient[l]);
                        }
                        else
                            printf("%5s ", "X");
                    }
                    else
                        printf("| %5s  :  %5s ", "X", "X");
                }
                printf("|\n\r");
            }
        }
    }

    for (i = 0; i < zones_per_line; i++)
    {
        printf(" -----------------");
    }
    printf("\n\r");
}

static void toggle_resolution_sensor(void)
{
    TOF_RANGING_SENSOR_Stop(CUSTOM_VL53L5CX_1);

    switch (RS_Profile.RangingProfile)
    {
        case RS_PROFILE_4x4_AUTONOMOUS:
            RS_Profile.RangingProfile = RS_PROFILE_8x8_AUTONOMOUS;
            break;

        case RS_PROFILE_4x4_CONTINUOUS:
            RS_Profile.RangingProfile = RS_PROFILE_8x8_CONTINUOUS;
            break;

        case RS_PROFILE_8x8_AUTONOMOUS:
            RS_Profile.RangingProfile = RS_PROFILE_4x4_AUTONOMOUS;
            break;

        case RS_PROFILE_8x8_CONTINUOUS:
            RS_Profile.RangingProfile = RS_PROFILE_4x4_CONTINUOUS;
            break;

        default:
            break;
    }

    TOF_RANGING_SENSOR_ConfigProfile(CUSTOM_VL53L5CX_1, &RS_Profile);
    TOF_RANGING_SENSOR_Start(CUSTOM_VL53L5CX_1, RS_MODE_BLOCKING_CONTINUOUS);
}

static void toggle_signal_and_ambient_sensor(void)
{
    TOF_RANGING_SENSOR_Stop(CUSTOM_VL53L5CX_1);

    RS_Profile.EnableAmbient = (RS_Profile.EnableAmbient) ? 0U : 1U;
    RS_Profile.EnableSignal = (RS_Profile.EnableSignal) ? 0U : 1U;

    TOF_RANGING_SENSOR_ConfigProfile(CUSTOM_VL53L5CX_1, &RS_Profile);
    TOF_RANGING_SENSOR_Start(CUSTOM_VL53L5CX_1, RS_MODE_BLOCKING_CONTINUOUS);
}

static void clear_result_screen(void)
{
    /* clear Screen : 27 is ESC command */
    printf("%c[2J", 27);
}

static void display_cmd_banner(void)
{
    /* clear screen */
    printf("%c[2H", 27);

    printf("VL53L5CX Simple Ranging demo application\n\r");
#ifdef USE_VL53L5CX_BARE_DRIVER
    printf("Using direct calls to VL53L5CX bare driver API\n\r");
#endif
    printf("Polling mode\n\r");
    printf("----------------------------------------\n\n\r");

    printf("Use the following keys to control application\n\r");
    printf(" 'r' : change resolution\n\r");
    printf(" 's' : enable signal and ambient\n\r");
    printf(" 'c' : clear screen\n\r");
    printf("\n\r");
}

static void handle_command(uint8_t cmd)
{
    switch (cmd)
    {
        case 'r':
            toggle_resolution_sensor();
            clear_result_screen();
            break;

        case 's':
            toggle_signal_and_ambient_sensor();
            clear_result_screen();
            break;

        case 'c':
            clear_result_screen();
            break;

        default:
            break;
    }
}

static uint8_t get_user_key(void)
{
    uint8_t cmd = 0;

    // HAL_UART_Receive(&hcom_uart[COM1], &cmd, 1, HAL_MAX_DELAY);

    return cmd;
}

static uint32_t com_uart_has_data(void)
{
    // return __HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_RXNE);;
}

#ifdef USE_VL53L5CX_BARE_DRIVER
static uint8_t map_target_status_usr(uint8_t status)
{
  uint8_t ret;

  if ((status == 5U) || (status == 9U))
  {
    ret = 0U; /* ranging is OK */
  }
  else if (status == 0U)
  {
    ret = 255U; /* no update */
  }
  else
  {
    ret = status; /* return device status otherwise */
  }

  return ret;
}

static int32_t convert_data_format_usr(VL53L5CX_Object_t *pObj,
                                   VL53L5CX_ResultsData *data, RANGING_SENSOR_Result_t *pResult)
{
  int32_t ret;
  uint8_t i, j;
  uint8_t resolution;
  uint8_t target_status;

  if ((pObj == NULL) || (pResult == NULL))
  {
    ret = VL53L5CX_INVALID_PARAM;
  }
  else if (vl53l5cx_get_resolution(&pObj->Dev, &resolution) != VL53L5CX_STATUS_OK)
  {
    ret = VL53L5CX_ERROR;
  }
  else
  {
    pResult->NumberOfZones = resolution;

    for (i = 0; i < resolution; i++)
    {
      pResult->ZoneResult[i].NumberOfTargets = data->nb_target_detected[i];

      for (j = 0; j < data->nb_target_detected[i]; j++)
      {
        pResult->ZoneResult[i].Distance[j] = (uint32_t)data->distance_mm[(VL53L5CX_NB_TARGET_PER_ZONE * i) + j];

        /* return Ambient value if ambient rate output is enabled */
        if (pObj->IsAmbientEnabled == 1U)
        {
          /* apply ambient value to all targets in a given zone */
          pResult->ZoneResult[i].Ambient[j] = (float_t)data->ambient_per_spad[i];
        }
        else
        {
          pResult->ZoneResult[i].Ambient[j] = 0.0f;
        }

        /* return Signal value if signal rate output is enabled */
        if (pObj->IsSignalEnabled == 1U)
        {
          pResult->ZoneResult[i].Signal[j] =
            (float_t)data->signal_per_spad[(VL53L5CX_NB_TARGET_PER_ZONE * i) + j];
        }
        else
        {
          pResult->ZoneResult[i].Signal[j] = 0.0f;
        }

        target_status = data->target_status[(VL53L5CX_NB_TARGET_PER_ZONE * i) + j];
        pResult->ZoneResult[i].Status[j] = map_target_status_usr(target_status);
      }
    }

    ret = VL53L5CX_OK;
  }

  return ret;
}
#endif

#ifdef __cplusplus
}
#endif



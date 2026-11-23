/*
 * tof_sensor_helper.cpp
 *
 *  Created on: Feb 8, 2025
 *      Author: root
 */
#include "../../Core/Inc/User/tof_sensor_helper.hpp"

static int32_t rs_status = 0; // Nur in diese Datei Sichtbar

bool VL53L5CX_SimpleRanging_Secure_Init(const uint32_t &instance, const uint16_t &address) {
    rs_status = TOF_RANGING_SENSOR_Init(instance, address);

    if (rs_status != BSP_ERROR_NONE)
    {
        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
            printf("[ERROR] SENSOR %ld Init failed\n\r", instance + 1);
        }
        return false;
        // while (true);
    }
    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {
        printf("[INFO] Sensor %ld Init Erfolgreich\n\r", instance + 1);
    }
    return true;
}

bool Secure_Set_Sensor_Address(const uint32_t &instance, const uint16_t &new_address) {


    if (DEFINED_LOG_LEVEL >= LOG_LEVEL::DEBUG_LEVEL) {
        printf("[DEBUG] Sensor %d Address set to 0x%04X\n\r", instance + 1, new_address);
    }

    /// Phase 4: Adresse von Sensor 1 mit der neuen Adresse new_address_1 0x49 setzen
    rs_status = TOF_RANGING_SENSOR_SetAddress(instance, new_address);
    if (rs_status != BSP_ERROR_NONE)
    {
        if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::ERROR_LEVEL) {
            printf("[ERROR] Sensor %ld set Address  failed\n\r", instance + 1);
        }
        return false;
        // while (true);
    }
    if constexpr (DEFINED_LOG_LEVEL >= LOG_LEVEL::INFO_LEVEL) {

        printf("[INFO] Sensor %ld set Address Erfolgreich\n\r", instance + 1);
    }

    return true;

    /// [DEBUG] Wir deinitialiseren den Sensor und initialisieren es nochmal um zu überprüfen, ob die Adresse sich nach
    /// der Deinitialisierung ändert oder nicht(Ergebniss: Die Adresse ändert sich nicht, es sei dann man resetet PWR_EN)
    /*
    rs_status = TOF_RANGING_SENSOR_DeInit(instance);
    if (rs_status != BSP_ERROR_NONE)
    {
        printf("\n\rSensor %d DeInit failed\n\r", instance);
        while (true);
    }
    printf("\n\rSensor %d DeInit Erfolgreich\n\r", instance);
    */
    /*
    rs_status = TOF_RANGING_SENSOR_Init(instance, new_address_1);

    if (rs_status != BSP_ERROR_NONE)
    {
        printf("\n\rCUSTOM_RANGING_SENSOR_Init %d failed\n\r", instance);
        while (true);
    }
    printf("\n\rSensor %d init Erfolgreich\n\r", instance);
    */
}


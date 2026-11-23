/*
 * spi_dma.cpp
 *
 *  Created on: Feb 22, 2025
 *      Author: root
 */

#include "../../Core/Inc/User/spi_dma.hpp"

#include "spi.h"

#include "stm32h5xx_hal.h"
#include "usart.h"

constexpr std::size_t spi1_rx_buffer_size = 256;
constexpr std::size_t spi1_tx_buffer_size = 256;
// static  std::array<uint8_t, tx_buffer_size > spi_tx_buffer;
alignas(4) static  std::array<uint16_t, spi1_tx_buffer_size > spi1_tx_buffer;
// static  std::array<uint8_t, rx_buffer_size> spi_rx_buffer;
alignas(4) static  std::array<uint16_t, spi1_rx_buffer_size> spi1_rx_buffer;
// alignas(4)







constexpr std::size_t spi2_rx_buffer_size = 256;
constexpr std::size_t spi2_tx_buffer_size = 256;
// static  std::array<uint8_t, tx_buffer_size > spi_tx_buffer;
alignas(4) static  std::array<uint16_t, spi1_tx_buffer_size > spi2_tx_buffer;
// static  std::array<uint8_t, rx_buffer_size> spi_rx_buffer;
alignas(4) static  std::array<uint16_t, spi1_rx_buffer_size> spi2_rx_buffer;
// alignas(4)





constexpr std::array<uint16_t, 128> generateArray() {
    std::array<uint16_t, 128> arr{};
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = i * 1000;
    }
    return arr;
}

constexpr std::array<uint16_t, 128> myarray = generateArray();  // ✅ Compile-Zeit Initialisierung


static uint16_t index_now = 0;



void Start_SPI_DMA_Transfer_SPI1() {
    printf("\n🔄 [Start] SPI DMA Transfer wird gestartet...\r\n");




    // SPI DMA Status prüfen
    if (hspi1.State != HAL_SPI_STATE_READY) {
        printf("⚠️ SPI nicht bereit! Aktueller Zustand: %d\r\n", hspi1.State);
        return;
    }

    // Buffer-Inhalt ausgeben (vor dem Transfer)
    printf("📤 TX Buffer vor Übertragung: ");
    for (unsigned char i : spi1_tx_buffer) {
        printf("%02X ", i);
    }
    printf("\r\n");

    printf("📥 RX Buffer vor Übertragung: ");
    for (unsigned char i : spi1_rx_buffer) {
        printf("%02X ", i);
    }
    printf("\r\n");

    // SPI DMA Übertragung starten

    memset(spi1_rx_buffer.data(), 0, spi1_rx_buffer.size());  // RX Buffer vorher leeren



    if (hspi1.Init.FifoThreshold == SPI_FIFO_THRESHOLD_08DATA) {
        printf("hspi1.Init.FifoThreshold:\t 8 [Line: %d] [Function: %s]\n\r", __LINE__, __FUNCTION__);
    }

    if (hspi1.hdmatx->Init.SrcBurstLength == 16) {
        printf("hspi1.hdmatx->Init.SrcBurstLength:\t 2 [Line: %d] [Function: %s]\n\r", __LINE__, __FUNCTION__);
    }
/*
    hspi1.hdmatx->Init.SrcBurstLength = 1;
    hspi1.hdmatx->Init.DestBurstLength = 1;

    hspi1.hdmarx->Init.SrcBurstLength = 1;
    hspi1.hdmarx->Init.DestBurstLength = 1;



    hspi1.hdmatx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    hspi1.hdmatx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;

    hspi1.hdmarx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    hspi1.hdmarx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
*/
    hspi1.hdmatx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    hspi1.hdmatx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;

    hspi1.hdmarx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    hspi1.hdmarx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;


    memset(spi1_rx_buffer.data(), 0, spi1_rx_buffer.size());  // RX Buffer vorher leeren
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(&hspi1, reinterpret_cast<const uint8_t *>(spi1_tx_buffer.data()),
        reinterpret_cast<uint8_t *>(spi1_rx_buffer.data()), spi1_rx_buffer.size() * 2);

    printf("SPI State nach Init: %d\n\r", hspi1.State);

    if (status != HAL_OK) {
        printf("❌ SPI DMA Start Error! Status Code: 0x%08lX\n\r\n", status);

        // Fehlerdiagnose
        if (status == HAL_BUSY) {
            printf("⚠️ SPI ist belegt! Eventuell ist eine vorherige Übertragung noch aktiv.\r\n");
        } else if (status == HAL_ERROR) {
            printf("⚠️ SPI Hardware-Fehler! Eventuell ein Problem mit dem Bus oder DMA.\r\n");


        } else if (status == HAL_TIMEOUT) {
            printf("⚠️ SPI Timeout! Überprüfung der Kommunikation erforderlich.\r\n");
        }

        // SPI-DMA sicher stoppen und neu starten
        HAL_SPI_DMAStop(&hspi1);
        HAL_Delay(10);
        HAL_SPI_DeInit(&hspi1);
        MX_SPI1_Init();
        HAL_Delay(10);
        // HAL_SPI_MspInit(&hspi1);


        printf("🔄 SPI & DMA werden erneut gestartet...\r\n");

        // Zweiter Versuch
        memset(spi1_rx_buffer.data(), 0, spi1_rx_buffer.size());  // RX Buffer vorher leeren


        status = HAL_SPI_TransmitReceive_DMA(&hspi1, reinterpret_cast<const uint8_t *>(spi1_tx_buffer.data()),
            reinterpret_cast<uint8_t *>(spi1_rx_buffer.data()), spi1_rx_buffer.size() * 2);
        if (status != HAL_OK) {
            printf("❌ [Fehler] SPI DMA Restart fehlgeschlagen! System-Reset erforderlich!\r\n");
            Error_Handler();
        } else {
            printf("✅ SPI DMA Transfer erfolgreich nach Neustart!\r\n");
        }
    } else {
        printf("✅ SPI DMA Transfer erfolgreich gestartet!\r\n");
        printf("✅ SPI DMA Transfer erfolgreich gestartet!\r\n");
    }
}

void Start_SPI_DMA_Transfer_SPI2() {
    printf("\n🔄 [Start] SPI DMA Transfer wird gestartet...\r\n");




    // SPI DMA Status prüfen
    if (hspi2.State != HAL_SPI_STATE_READY) {
        printf("⚠️ SPI nicht bereit! Aktueller Zustand: %d\r\n", hspi2.State);
        return;
    }

    // Buffer-Inhalt ausgeben (vor dem Transfer)
    printf("📤 TX Buffer vor Übertragung: ");
    for (unsigned char i : spi1_tx_buffer) {
        printf("%02X ", i);
    }
    printf("\r\n");

    printf("📥 RX Buffer vor Übertragung: ");
    for (unsigned char i : spi1_rx_buffer) {
        printf("%02X ", i);
    }
    printf("\r\n");

    // SPI DMA Übertragung starten

   if (hspi2.Init.FifoThreshold == SPI_FIFO_THRESHOLD_08DATA) {
        printf("hspi2.Init.FifoThreshold:\t 8 [Line: %d] [Function: %s]\n\r", __LINE__, __FUNCTION__);
    }

    if (hspi2.hdmatx->Init.SrcBurstLength == 16) {
        printf("hspi2.hdmatx->Init.SrcBurstLength:\t 2 [Line: %d] [Function: %s]\n\r", __LINE__, __FUNCTION__);
    }
/*
    hspi2.hdmatx->Init.SrcBurstLength = 1;
    hspi2.hdmatx->Init.DestBurstLength = 1;

    hspi2.hdmarx->Init.SrcBurstLength = 1;
    hspi2.hdmarx->Init.DestBurstLength = 1;



    hspi2.hdmatx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    hspi2.hdmatx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;

    hspi2.hdmarx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    hspi2.hdmarx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
*/
    hspi2.hdmatx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_HALFWORD;
    hspi2.hdmatx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_HALFWORD;

    hspi2.hdmarx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_HALFWORD;
    hspi2.hdmarx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_HALFWORD;


    memset(spi2_rx_buffer.data(), 0, spi2_rx_buffer.size());  // RX Buffer vorher leeren
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(&hspi2, reinterpret_cast<const uint8_t *>(spi2_tx_buffer.data()),
        reinterpret_cast<uint8_t *>(spi2_rx_buffer.data()), spi2_rx_buffer.size() * 2);

    printf("SPI State nach Init: %d\n\r", hspi2.State);

    if (status != HAL_OK) {
        printf("❌ SPI DMA Start Error! Status Code: 0x%08lX\n\r\n", status);

        // Fehlerdiagnose
        if (status == HAL_BUSY) {
            printf("⚠️ SPI ist belegt! Eventuell ist eine vorherige Übertragung noch aktiv.\r\n");
        } else if (status == HAL_ERROR) {
            printf("⚠️ SPI Hardware-Fehler! Eventuell ein Problem mit dem Bus oder DMA.\r\n");


        } else if (status == HAL_TIMEOUT) {
            printf("⚠️ SPI Timeout! Überprüfung der Kommunikation erforderlich.\r\n");
        }

        // SPI-DMA sicher stoppen und neu starten
        HAL_SPI_DMAStop(&hspi2);
        HAL_Delay(10);
        HAL_SPI_DeInit(&hspi2);
        MX_SPI2_Init();
        HAL_Delay(10);
        // HAL_SPI_MspInit(&hspi1);


        printf("🔄 SPI & DMA werden erneut gestartet...\r\n");

        // Zweiter Versuch
        memset(spi2_rx_buffer.data(), 0, spi2_rx_buffer.size());  // RX Buffer vorher leeren


        status = HAL_SPI_TransmitReceive_DMA(&hspi2, reinterpret_cast<const uint8_t *>(spi2_tx_buffer.data()),
            reinterpret_cast<uint8_t *>(spi2_rx_buffer.data()), spi2_rx_buffer.size() * 2);
        if (status != HAL_OK) {
            printf("❌ [Fehler] SPI DMA Restart fehlgeschlagen! System-Reset erforderlich!\r\n");
            Error_Handler();
        } else {
            printf("✅ SPI DMA Transfer erfolgreich nach Neustart!\r\n");
        }
    } else {
        printf("✅ SPI DMA Transfer erfolgreich gestartet!\r\n");
    }
}


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    
    if (hspi->Instance == SPI1) {
/*
        printf("\n✅ [SPI Callback] SPI TX/RX abgeschlossen.\n\r");

        // 📥 Debugging: Empfangene RX-Daten als HEX ausgeben
        printf("📥 Empfangene RX-Daten (%d Werte): \n\r", spi_rx_buffer.size());
        for (int i = 0; i < spi_rx_buffer.size(); i++) {
            if (i % 16 == 0 and i != 0) {printf("\n\r");}
            printf("%04X ", spi_rx_buffer[i]);

        }
        printf("\r\n");



        printf("\r\n");
*/
        // 📤 Neue TX-Daten für den nächsten Transfer vorbereiten
        prepare_spi1_data();  // Holt VL53L5CX Distanzdaten

        // printf("🔄 Starte nächste SPI DMA Übertragung...\r\n");
        HAL_SPI_TransmitReceive_DMA(&hspi1, reinterpret_cast<const uint8_t *>(spi1_tx_buffer.data()),
             reinterpret_cast<uint8_t *>(spi1_rx_buffer.data()), spi1_rx_buffer.size() * 2);

    }

    else if (hspi->Instance == SPI2) {
        prepare_spi2_data();
        HAL_SPI_TransmitReceive_DMA(&hspi2, reinterpret_cast<const uint8_t *>(spi2_tx_buffer.data()),
                     reinterpret_cast<uint8_t *>(spi2_rx_buffer.data()), spi2_rx_buffer.size() * 2);
    }
}



uint16_t crc16(const uint16_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];  // XOR mit Daten
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;  // CRC-16 Polynom
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}





void delay_loop(uint32_t us) {
    uint32_t clock_mhz = SystemCoreClock / 1000000;  // Umrechnung auf MHz
    volatile uint32_t count = clock_mhz * us / 8;  // Präzisere Berechnung für Cortex-M33
    while (count--) {
        __NOP();
    }
}
void delay_non_blocking(uint32_t us) {
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < (us / 1000));  // Mikrosekunden → Millisekunden umgerechnet
}


/*
 * Anmerkung: In Zukunft könnte man Sensor 1 und 2 abrufen und beim nächsten Transfer abwechselnd Sensor 3 und 4 usw.
 *
 *
 */
void prepare_spi1_data() {
    memset(spi1_tx_buffer.data(), 0, spi1_tx_buffer.size() * sizeof(uint16_t)); // Setzt alles auf 0

    static VL53L5CX_ResultsData data1, data2, data3, data4;
    uint8_t NewDataReady1 = 0, NewDataReady2 = 0, NewDataReady3 = 0, NewDataReady4 = 0;


    uint16_t *spi1_data = spi1_tx_buffer.data();



    // Prüfen, ob neue Daten für Sensoren vorhanden sind
    if (is_initialized[1] == true) {
        (void)vl53l5cx_check_data_ready(&(pL5obj[VL53L5CX_1]->Dev), &NewDataReady1);
        if (NewDataReady1) {
            vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_1]->Dev), &data1);
        }
        memcpy(&spi1_data[32], data1.distance_mm, 64 * sizeof(uint16_t));
    }
    else {
        memset(&spi1_data[32], 1, 64 * sizeof(uint16_t));
    }


    if (is_initialized[3] == true) {
        (void)vl53l5cx_check_data_ready(&(pL5obj[VL53L5CX_3]->Dev), &NewDataReady3);
        if (NewDataReady3) {
            vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_3]->Dev), &data3);
        }
        memcpy(&spi1_data[96], data3.distance_mm, 64 * sizeof(uint16_t));
    }
    else {
        memset(&spi1_data[96], 1, 64 * sizeof(uint16_t));
    }


    if (is_initialized[4] == true) {
        (void)vl53l5cx_check_data_ready(&(pL5obj[VL53L5CX_4]->Dev), &NewDataReady4);
        if (NewDataReady4) {
            vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_4]->Dev), &data4);
        }

        memcpy(&spi1_data[160], data4.distance_mm, 64 * sizeof(uint16_t));
    }
    else {
        memset(&spi1_data[160], 1, 64 * sizeof(uint16_t));
    }

    // ✅ CRC-16 Prüfsumme berechnen und an Index 288 speichern
    spi1_data[224] = crc16(spi1_data, 224);

    // Debugging
    printf("CRC: %04X\n\r", spi1_data[224]);
    printf("CRC: %04X\n\r", spi1_data[224]);
}


void prepare_spi2_data() {
    memset(spi2_tx_buffer.data(), 0, spi2_tx_buffer.size() * sizeof(uint16_t)); // Setzt alles auf 0

    VL53L5CX_ResultsData data1, data2;
    uint8_t NewDataReady1 = 0, NewDataReady2 = 0;




    // SPI-Buffer vorbereiten
    uint16_t *spi2_data = spi2_tx_buffer.data();

    // ✅ Die ersten 32 Werte (zwei Zeilen) bleiben 0




    // Prüfen, ob neue Daten für Sensoren vorhanden sind
    if (is_initialized[3] == true) {
        (void)vl53l5cx_check_data_ready(&(pL5obj[VL53L5CX_3]->Dev), &NewDataReady1);
        if (NewDataReady1) {
            vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_3]->Dev), &data1);
        }
        //else {
        //    vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_1]->Dev), &data1);
        //}
        memcpy(&spi2_data[32], data1.distance_mm, 64 * sizeof(uint16_t));

    }
    else {
        memset(&spi2_data[32], 0, 64 * sizeof(uint16_t));
    }

    if (is_initialized[4] == true) {
        (void)vl53l5cx_check_data_ready(&(pL5obj[VL53L5CX_4]->Dev), &NewDataReady2);
        if (NewDataReady2) {
            vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_4]->Dev), &data2);
        }
        //else {
        //    vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_2]->Dev), &data2);
        //}
        memcpy(&spi2_data[96], data2.distance_mm, 64 * sizeof(uint16_t));
    }
    else {
        memset(&spi2_data[96], 0, 64 * sizeof(uint16_t));
    }


    // ✅ Daten für Sensor 1 (64 Werte, beginnend ab Index 32)
    //for (int i = 0; i < 64; i++) {
    //    spi_data[32 + i] = data1.distance_mm[i];
    //}

    // ✅ Daten für Sensor 2 (64 Werte, beginnend ab Index 96)
    //for (int i = 0; i < 64; i++) {
    //    spi_data[96 + i] = data2.distance_mm[i];
    //}

    // ✅ CRC-16 Prüfsumme berechnen und an Index 160 speichern
    spi2_data[160] = crc16(spi2_data, 160);

    // Warte auf DMA

    // delay_non_blocking(1);  // 4 ms Verzögerung

    // Debugging
    //printf("\n\r");
    //printf("CRC: %04X\n\r", spi2_data[160]);
    // printf("CRC: %04X\n\r", spi2_data[160]);

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "CRC: %04X\n\r", spi2_data[160]);
    HAL_UART_Transmit(&huart3, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

    //printf("📤 SPI TX-Daten vorbereitet (%d Werte + CRC: %04X):\n\r", 161, spi_data[160]);
    //for (int i = 0; i < 161; i++) {
    //    if (i % 16 == 0 && i != 0) { printf("\n\r"); }
    //    printf("%04X ", spi_data[i]);
    //}
    //printf("\r\n");
}

/*
// Für zwei Sensoren VL53L5CX_1, VL53L5CX_2
void prepare_spi1_data() {


    memset(spi1_tx_buffer.data(), 0, spi1_tx_buffer.size() * sizeof(uint16_t)); // Setzt alles auf 0

    VL53L5CX_ResultsData data1, data2;
    uint8_t NewDataReady1 = 0, NewDataReady2 = 0;

    // SPI-Buffer vorbereiten
    uint16_t *spi1_data = spi1_tx_buffer.data();

    // ✅ Die ersten 32 Werte (zwei Zeilen) bleiben 0





    // Prüfen, ob neue Daten für Sensoren vorhanden sind
    if (is_initialized[1] == true) {
        (void)vl53l5cx_check_data_ready(&(pL5obj[VL53L5CX_1]->Dev), &NewDataReady1);
        if (NewDataReady1) {
            vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_1]->Dev), &data1);
        }
        //else {
        //    vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_1]->Dev), &data1);
        //}
        memcpy(&spi1_data[32], data1.distance_mm, 64 * sizeof(uint16_t));
    }
    else {
        memset(&spi1_data[32], 1, 64 * sizeof(uint16_t));
    }

    if (is_initialized[2] == true) {
        (void)vl53l5cx_check_data_ready(&(pL5obj[VL53L5CX_2]->Dev), &NewDataReady2);
        if (NewDataReady2) {
            vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_2]->Dev), &data2);
        }
        //else {
        //    vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_2]->Dev), &data2);
        //}
        memcpy(&spi1_data[96], data2.distance_mm, 64 * sizeof(uint16_t));
    }
    else {
        memset(&spi1_data[96], 1, 64 * sizeof(uint16_t));
    }


    // ✅ Daten für Sensor 1 (64 Werte, beginnend ab Index 32)
    //for (int i = 0; i < 64; i++) {
    //    spi_data[32 + i] = data1.distance_mm[i];
    //}

    // ✅ Daten für Sensor 2 (64 Werte, beginnend ab Index 96)
    //for (int i = 0; i < 64; i++) {
    //    spi_data[96 + i] = data2.distance_mm[i];
    //}

    // ✅ CRC-16 Prüfsumme berechnen und an Index 160 speichern
    spi1_data[160] = crc161(spi1_data, 160);

    // Debugging
    // delay_non_blocking(300);  // 4 ms Verzögerung

    printf("CRC: %04X\n\r", spi1_data[160]);
    printf("CRC: %04X\n\r", spi1_data[160]);
    // printf("📤 SPI TX-Daten vorbereitet (%d Werte + CRC: %04X):\n\r", 161, spi_data[160]);
    //for (int i = 0; i < 161; i++) {
    //    if (i % 16 == 0 && i != 0) { printf("\n\r"); }
    //    printf("%04X ", spi_data[i]);

    //}
    //printf("\r\n");


}
*/







/*
static const uint16_t crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040,

};
*/
/*
void prepare_spi_data() {
    static VL53L5CX_ResultsData data1, data2; // Ergebnisse für beide Instanzen
    uint8_t NewDataReady1 = 0, NewDataReady2 = 0;
    
    // Prüfe, ob neue Daten für Sensor 1 vorhanden sind
    (void)vl53l5cx_check_data_ready(&(pL5obj[VL53L5CX_1]->Dev), &NewDataReady1);
    if (NewDataReady1) {
        vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_1]->Dev), &data1);
    }

    // Prüfe, ob neue Daten für Sensor 2 vorhanden sind
    (void)vl53l5cx_check_data_ready(&(pL5obj[VL53L5CX_2]->Dev), &NewDataReady2);
    if (NewDataReady2) {
        vl53l5cx_get_ranging_data(&(pL5obj[VL53L5CX_2]->Dev), &data2);
    }

    // SPI-Datenbuffer vorbereiten
    uint16_t *spi_data = reinterpret_cast<uint16_t*>(spi_tx_buffer.data());

    // Daten für Sensor 1 übertragen
    for (int i = 0; i < VL53L5CX_RESOLUTION_8X8; i++) {
        spi_data[i] = data1.distance_mm[i]; // 16-bit Distanzwerte
    }

    // Daten für Sensor 2 übertragen
    for (int i = 0; i < VL53L5CX_RESOLUTION_8X8; i++) {
        spi_data[VL53L5CX_RESOLUTION_8X8 + i] = data2.distance_mm[i];
    }

    printf("📤 SPI TX-Daten vorbereitet (%d Bytes):\n", spi_tx_buffer.size());
    for (int i = 0; i < spi_tx_buffer.size(); i += 2) {
        printf("%04X ", spi_data[i / 2]); // 16-bit Werte ausgeben
    }
    printf("\r\n");
}
*/


/*
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {



        // 📤 Neue TX-Daten für den nächsten Transfer vorbereiten
        snprintf(reinterpret_cast<char *>(spi_tx_buffer.data()), spi_tx_buffer.size(), "DATA TICK: %lu", myarray[index_now]);
        HAL_SPI_TransmitReceive_DMA(&hspi1, reinterpret_cast<const uint8_t *>(spi_tx_buffer.data()), reinterpret_cast<uint8_t *>(spi_rx_buffer.data()), spi_rx_buffer.size());



        index_now++;

        if (index_now >= myarray.size()) {
            index_now = 0;
        }



        // printf("\n✅ [SPI Callback] Übertragung abgeschlossen! RX/TX erfolgreich.\n\r");

        // 📥 Debugging: Empfangene RX-Daten als HEX ausgeben

        printf("📥 Empfangene RX-Daten (%d Bytes): ", spi_rx_buffer.size());
        for (unsigned char i : spi_rx_buffer) {
            printf("%02X ", i);
        }

        print("\r\n");

        // 📥 Empfangene RX-Daten als String
        // spi_rx_buffer[spi_rx_buffer.size() - 1] = '\0';  // Null-Terminierung
        // printf("📝 Empfangen als String: \"%s\"\r\n", reinterpret_cast<char*>(spi_rx_buffer.data()));



        //if (std::ranges::all_of(spi_rx_buffer, [](uint8_t b) { return b == 0x00; })) {
        //    printf("⚠️ WARNUNG: Empfangene Daten sind leer oder ungültig!\r\n");
        //}

        //index_now++;
        //if (index_now >= myarray.size()) {
        //    index_now = 0;
        //}

        // 📤 Neue TX-Daten für den nächsten Transfer vorbereiten

        // snprintf(reinterpret_cast<char *>(spi_tx_buffer.data()), spi_tx_buffer.size(), "DATA TICK: %lu", myarray[index_now]);
        // VL53L5CX_TOF_Process(VL53L5CX_1);
        // snprintf(reinterpret_cast<char *>(spi_tx_buffer.data()), spi_tx_buffer.size(), "DATA TICK: %lu", HAL_GetTick());

        // 📤 Debugging: TX-Daten anzeigen
        //printf("📤 Gesendete TX-Daten (%d Bytes): \"%s\"\r\n", spi_tx_buffer.size(),
        //    reinterpret_cast<char *>(spi_tx_buffer.data()));
        printf("%d\n\r", myarray[index_now]);
        // 🔄 SPI-DMA RX neu starten, um auf neue Daten zu warten
        // printf("🔄 Starte nächste SPI DMA Übertragung...\r\n");
        // HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer.data(), spi_rx_buffer.data(), spi_rx_buffer.size());
    }
}

*/


// 📥 Empfang vollständig abgeschlossen

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    printf("✅ [SPI] HAL_SPI_RxCpltCallback: Empfang vollständig abgeschlossen! SPI-Instance: %p\n\r", hspi);
}

// 📤 Übertragung vollständig abgeschlossen
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    printf("✅ [SPI] HAL_SPI_TxCpltCallback: Senden vollständig abgeschlossen! SPI-Instance: %p\n\r", hspi);
}

// 📤📥 Halb-duplex-Übertragung vollständig abgeschlossen


// 📤📥 Halb-duplex-Übertragung zur Hälfte abgeschlossen (z. B. für Double Buffering)
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi) {
    // printf("🔄 [SPI] HAL_SPI_TxRxHalfCpltCallback: TX/RX zur Hälfte abgeschlossen. SPI-Instance: %p\n\r", hspi);
}

// ⚠️ Fehler aufgetreten
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1 or hspi->Instance == SPI2) {
        printf("\n❌ [SPI] HAL_SPI_ErrorCallback: SPI-Fehler erkannt!\n");
        printf("🔎 Fehlercode: 0x%08X | SPI-Instance: %p\n", hspi->ErrorCode, hspi->Instance);

        // 📌 1️⃣ Overrun-Fehler (RX-Puffer wurde nicht rechtzeitig geleert)
        if (hspi->ErrorCode & HAL_SPI_ERROR_OVR) {
            printf("⚠️ [SPI] Overrun-Fehler erkannt! RX-Puffer war nicht rechtzeitig geleert.\n");
            __HAL_SPI_CLEAR_OVRFLAG(hspi);  // Overrun-Flag zurücksetzen
        }

        // 📌 2️⃣ Mode-Fault-Fehler (Multi-Master-Kollision)
        if (hspi->ErrorCode & HAL_SPI_ERROR_MODF) {
            printf("🚨 [SPI] Mode-Fault-Fehler erkannt! Multi-Master-Kollision oder falsche CS-Steuerung.\n");
            __HAL_SPI_CLEAR_MODFFLAG(hspi);
        }

        // 📌 3️⃣ CRC-Fehler (Fehlende oder beschädigte CRC-Prüfung)
        if (hspi->ErrorCode & HAL_SPI_ERROR_CRC) {
            printf("⚠️ [SPI] CRC-Fehler erkannt! Übertragene Daten könnten beschädigt sein.\n");
            __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
        }

        // 📌 4️⃣ Frame-Format-Fehler (Falsche SPI-Einstellungen)
        if (hspi->ErrorCode & HAL_SPI_ERROR_FRE) {
            printf("❌ [SPI] Frame-Format-Fehler erkannt! Möglicherweise falsche SPI-Einstellungen (CPOL, CPHA, Datenlänge).\n");
            __HAL_SPI_CLEAR_FREFLAG(hspi);
        }

        // 📌 5️⃣ DMA-Fehler (Falls SPI mit DMA läuft)
        if (hspi->ErrorCode & HAL_SPI_ERROR_DMA) {
            printf("❌ [SPI] DMA-Fehler erkannt! Überprüfung der DMA-Konfiguration erforderlich.\n");
        }

        // 📌 6️⃣ Unbekannter Fehler (Falls kein bekannter Fehlercode zutrifft)
        if (hspi->ErrorCode == HAL_SPI_ERROR_NONE) {
            printf("✅ [SPI] Kein Fehler erkannt, aber Callback wurde trotzdem aufgerufen.\n");
        }

        // 🔄 SPI-Übertragung sicher neu starten
        printf("🔄 [SPI] Abbruch & Neustart der SPI-DMA-Übertragung...\n");

        HAL_SPI_DMAStop(&hspi1);
        HAL_Delay(10);
        HAL_SPI_DeInit(&hspi1);
        MX_SPI1_Init();
        HAL_Delay(10);



        HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(
            &hspi1,
            reinterpret_cast<const uint8_t *>(spi1_tx_buffer.data()),
            reinterpret_cast<uint8_t *>(spi1_rx_buffer.data()),
            spi1_rx_buffer.size() * 2
        );

        if (status == HAL_OK) {
            printf("✅ [SPI] SPI-DMA-Übertragung erfolgreich neu gestartet.\n");
        } else {
            printf("❌ [SPI] Fehler beim Neustart der SPI-DMA-Übertragung! Fehlercode: %d\n", status);
        }

        printf("\n");
    }
}



// 🛑 SPI-Abbruch abgeschlossen
void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi) {
    printf("🛑 [SPI] HAL_SPI_AbortCpltCallback: SPI-Übertragung wurde erfolgreich abgebrochen! SPI-Instance: %p\n\r", hspi);
}

// 🔄 SPI wurde pausiert / suspendiert
void HAL_SPI_SuspendCallback(SPI_HandleTypeDef *hspi) {
    printf("⏸ [SPI] HAL_SPI_SuspendCallback: SPI wurde pausiert. SPI-Instance: %p\n\r", hspi);
}














/*
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        printf("\n✅ [SPI Callback] Übertragung abgeschlossen! RX/TX erfolgreich.\r\n");

        // 📥 Debugging: Empfangene RX-Daten als HEX ausgeben
        printf("📥 Empfangene RX-Daten (%zu Bytes): ", spi_rx_buffer.size());
        for (unsigned char i : spi_rx_buffer) {
            printf("%02X ", i);
        }
        printf("\r\n");

        // 📥 Empfangene RX-Daten als String
        //spi_rx_buffer[spi_rx_buffer.size() - 1] = '\0';  // Null-Terminierung
        //printf("📝 Empfangen als String: \"%s\"\r\n", reinterpret_cast<char*>(spi_rx_buffer.data()));


        // 📥 Empfangene RX-Daten als String sicher ausgeben
        std::string received_str(spi_rx_buffer.begin(), spi_rx_buffer.end());
        printf("📝 Empfangen als String: \"%s\"\r\n", received_str.c_str());


        // ⚠️ Fehlerprüfung: Ist der gesamte Puffer leer?
        if (std::ranges::all_of(spi_rx_buffer, [](uint8_t b) { return b == 0x00; })) {
            printf("⚠️ WARNUNG: Empfangene Daten sind leer oder ungültig!\r\n");
        }




        // 📤 Neue TX-Daten für den nächsten Transfer vorbereiten

        snprintf(reinterpret_cast<char *>(spi_tx_buffer.data()), spi_tx_buffer.size(), "DATA TICK: %lu", HAL_GetTick());

        // 📤 Debugging: TX-Daten anzeigen
        printf("📤 Gesendete TX-Daten (%zu Bytes): \"%s\"\r\n", spi_tx_buffer.size(),
            reinterpret_cast<char *>(spi_tx_buffer.data()));

        // 🔄 SPI-DMA RX neu starten, um auf neue Daten zu warten
        printf("🔄 Starte nächste SPI DMA Übertragung...\r\n");
        HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer.data(), spi_rx_buffer.data(), spi_rx_buffer.size());
    }
}
*/




/*
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        printf("\n✅ [SPI Callback] Übertragung abgeschlossen! RX/TX erfolgreich.\r\n");

        printf("📥 Empfangene RX-Daten (%zu Bytes): ", spi_rx_buffer.size());
        for (unsigned char i : spi_rx_buffer) {
            printf("%02X ", i);
        }
        printf("\r\n");

        std::string received_str(spi_rx_buffer.begin(), spi_rx_buffer.end());
        printf("📝 Empfangen als String: \"%s\"\r\n", received_str.c_str());

        if (std::all_of(spi_rx_buffer.begin(), spi_rx_buffer.end(), [](uint8_t b) { return b == 0x00; })) {
            printf("⚠️ WARNUNG: Empfangene Daten sind leer oder ungültig!\r\n");
        }



        // 📤 Neue TX-Daten vorbereiten
        snprintf(reinterpret_cast<char *>(spi_tx_buffer.data()), spi_tx_buffer.size(), "DATA TICK: %lu", HAL_GetTick());
        printf("📤 Gesendete TX-Daten (%zu Bytes): \"%s\"\r\n", spi_tx_buffer.size(), reinterpret_cast<char *>(spi_tx_buffer.data()));

        printf("🔄 SPI-DMA Übertragung wird neu gestartet...\r\n");
        if (HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer.data(), spi_rx_buffer.data(), spi_rx_buffer.size()) != HAL_OK) {
            printf("❌ FEHLER: SPI-DMA Start fehlgeschlagen!\r\n");
        } else {
            printf("✅ SPI-DMA erfolgreich neu gestartet.\r\n");
        }
    }
}
*/


/*
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        printf("\n✅ [SPI Callback] Übertragung abgeschlossen! RX/TX erfolgreich.\r\n");

        // 📥 Debugging: Empfangene RX-Daten als HEX ausgeben
        printf("📥 Empfangene RX-Daten (%zu Bytes): ", spi_rx_buffer.size());
        for (size_t i = 0; i < spi_rx_buffer.size(); ++i) {
            printf("%02X ", spi_rx_buffer[i]);
        }
        printf("\r\n");

        // 📥 Empfangene RX-Daten als String sicher ausgeben
        std::string received_str(spi_rx_buffer.begin(), spi_rx_buffer.end());
        printf("📝 Empfangen als String: \"%s\"\r\n", received_str.c_str());

        // ⚠️ Fehlerprüfung: Ist der gesamte Puffer leer?
        if (std::all_of(spi_rx_buffer.begin(), spi_rx_buffer.end(), [](uint8_t b) { return b == 0x00; })) {
            printf("⚠️ WARNUNG: Empfangene Daten sind leer oder ungültig!\r\n");
        }

        // 📤 Neue TX-Daten für den nächsten Transfer vorbereiten
        // snprintf(reinterpret_cast<char*>(spi_tx_buffer.data()), spi_tx_buffer.size(), "DATA TICK: %lu", HAL_GetTick());







        // 📤 Debugging: TX-Daten anzeigen
        printf("📤 Gesendete TX-Daten (%zu Bytes): \"%s\"\r\n", spi_tx_buffer.size(),
               reinterpret_cast<char*>(spi_tx_buffer.data()));

        // 🔄 SPI-DMA RX neu starten, um auf neue Daten zu warten
        printf("🔄 Starte nächste SPI DMA Übertragung...\r\n");
        HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer.data(), spi_rx_buffer.data(), spi_rx_buffer.size());
    }
}
*/





/*
*
*FUNKTIONIERT
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        printf("\n✅ [SPI Callback] Übertragung abgeschlossen! RX/TX erfolgreich.\r\n");

        // 📥 Debugging: Empfangene RX-Daten als HEX ausgeben
        printf("📥 Empfangene RX-Daten (%d Bytes): ", spi_rx_buffer.size());
        for (unsigned char i : spi_rx_buffer) {
            printf("%02X ", i);
        }
        printf("\r\n");

        // 📥 Empfangene RX-Daten als String
        spi_rx_buffer[spi_rx_buffer.size() - 1] = '\0';  // Null-Terminierung
        printf("📝 Empfangen als String: \"%s\"\r\n", reinterpret_cast<char*>(spi_rx_buffer.data()));

        // ⚠️ Fehlerprüfung: Ist der Puffer leer?
        if (spi_rx_buffer[0] == 0x00) {
            printf("⚠️ WARNUNG: Empfangene Daten sind leer oder ungültig!\r\n");
        }

        // 📤 Neue TX-Daten für den nächsten Transfer vorbereiten

        snprintf(reinterpret_cast<char *>(spi_tx_buffer.data()), spi_tx_buffer.size(), "DATA TICK: %lu", HAL_GetTick());

        // 📤 Debugging: TX-Daten anzeigen
        printf("📤 Gesendete TX-Daten (%d Bytes): \"%s\"\r\n", spi_tx_buffer.size(),
            reinterpret_cast<char *>(spi_tx_buffer.data()));

        // 🔄 SPI-DMA RX neu starten, um auf neue Daten zu warten
        printf("🔄 Starte nächste SPI DMA Übertragung...\r\n");
        HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer.data(), spi_rx_buffer.data(), spi_rx_buffer.size());
    }
}






// Ausführliche

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        printf("\n✅ [SPI Callback] Übertragung abgeschlossen! RX/TX abgeschlossen.\r\n");

        //📥 Debugging: Empfangene Daten anzeigen
        printf("📥 Empfangene RX Daten (%d Bytes): ", RX_BUFFER_SIZE);
        for (int i = 0; i < RX_BUFFER_SIZE; i++) {
            printf("%02X ", spi_rx_buffer[i]);
        }
        printf("\r\n");

        // 📤 Debugging: Gesendete Daten anzeigen
        printf("📤 Gesendete TX Daten (%d Bytes): ", TX_BUFFER_SIZE);
        for (int i = 0; i < TX_BUFFER_SIZE; i++) {
            printf("%02X ", spi_tx_buffer[i]);
        }
        printf("\r\n");

        // 📌 Datenvalidierun
        if (RX_BUFFER_SIZE == 0 || spi_rx_buffer[0] == 0x00) {
            printf("⚠️ WARNUNG: Empfangene Daten sind leer oder ungültig!\r\n");
        }

        // 🛠 Fehlerdiagnose: Falls SPI TX/RX nicht erfolgreich war
        if (hspi->ErrorCode != HAL_SPI_ERROR_NONE) {
            printf("❌ [FEHLER] SPI Fehler erkannt! Fehlercode: 0x%02X\r\n", hspi->ErrorCode);

            if (hspi->ErrorCode & HAL_SPI_ERROR_CRC) {
                printf("🔍 CRC-Fehler! Überprüfe SPI-Verbindungen oder Taktsignal.\r\n");
            }
            if (hspi->ErrorCode & HAL_SPI_ERROR_OVR) {
                printf("🔍 Overrun-Fehler! Master liest Daten schneller als Slave senden kann.\r\n");
            }
            if (hspi->ErrorCode & HAL_SPI_ERROR_MODF) {
                printf("🔍 Modusfehler! Möglicherweise ist NSS falsch konfiguriert.\r\n");
            }
            if (hspi->ErrorCode & HAL_SPI_ERROR_FRE) {
                printf("🔍 Frame-Fehler! SPI-Takt oder Datenpakete könnten beschädigt sein.\r\n");
            }
            if (hspi->ErrorCode & HAL_SPI_ERROR_UDR) {
                printf("🔍 Underrun-Fehler! Der Slave sendet langsamer als erwartet.\r\n");
            }

            // Versuche SPI zu resetten
            HAL_SPI_Abort(hspi);
            HAL_SPI_DeInit(hspi);
            MX_SPI1_Init();
            HAL_Delay(10);

            printf("🔄 SPI-Peripherie zurückgesetzt, starte Übertragung erneut...\r\n");
            Start_SPI_DMA_Transfer();
            return;
        }

        // 📌 Bereite neue Antwort für den nächsten Transfer vor
        snprintf((char*)spi_tx_buffer, TX_BUFFER_SIZE, "TICK:%lu", HAL_GetTick());

        // 🔄 Starte den nächsten SPI DMA Transfer (nur falls nicht zirkulär konfiguriert)
        printf("🔄 Starte nächste SPI DMA Übertragung...\r\n");
        Start_SPI_DMA_Transfer();
    }
}



void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        printf("\n❌ SPI Fehler erkannt! Fehlercode: 0x%02X\n\r", hspi->ErrorCode);

        // Clock-Frequenz überprüfen
        printf("🔍 SPI1 Clock: %lu Hz\n\r", HAL_RCC_GetPCLK2Freq());

        // SPI-Statusregister auslesen (nur für Debugging)
        uint32_t spi_sr = hspi->Instance->SR;
        printf("🔍 SPI1 Status Register (SR): 0x%08X\n\r", spi_sr);

        // Fehlerursachen ausgeben
        if (hspi->ErrorCode & HAL_SPI_ERROR_MODF) {
            printf("⚠️  Mode Fault: Check if another SPI Master is active!\n\r");
        }
        if (hspi->ErrorCode & HAL_SPI_ERROR_CRC) {
            printf("⚠️  CRC Error: Data integrity issue detected!\n\r");
        }
        if (hspi->ErrorCode & HAL_SPI_ERROR_OVR) {
            printf("⚠️  Overrun Error: Master might be too fast!\n\r");
        }
        if (hspi->ErrorCode & HAL_SPI_ERROR_FRE) {
            printf("⚠️  Frame Format Error: Check SPI settings (CPOL, CPHA, NSS)!\n\r");
        }
        if (hspi->ErrorCode & HAL_SPI_ERROR_DMA) {
            printf("⚠️  DMA Transfer Error: Check DMA configuration!\n\r");
        }
        if (hspi->ErrorCode & HAL_SPI_ERROR_FLAG) {
            printf("⚠️  Unknown Flag Error detected!\n\r");
        }

        // DMA Status ausgeben
        printf("🔍 DMA RX Status: Active: %d, State: %d, Error: %d\n\r",
               HAL_DMA_GetState(hspi->hdmarx), hspi->hdmarx->State, hspi->hdmarx->ErrorCode);
        printf("🔍 DMA TX Status: Active: %d, State: %d, Error: %d\n\r",
               HAL_DMA_GetState(hspi->hdmatx), hspi->hdmatx->State, hspi->hdmatx->ErrorCode);

        // TX- & RX-Buffer-Inhalt debuggen
        printf("🔍 TX Buffer: ");
        for (int i = 0; i < TX_BUFFER_SIZE; i++) {
            printf("%02X ", spi_tx_buffer[i]);
        }
        printf("\n\r");

        printf("🔍 RX Buffer: ");
        for (int i = 0; i < RX_BUFFER_SIZE; i++) {
            printf("%02X ", spi_rx_buffer[i]);
        }
        printf("\n\r");

        // 🛠 Fehlerbehebung durch sicheren Neustart
        printf("🔄 SPI & DMA werden neu initialisiert...\n\r");

        // SPI und DMA sicher stoppen
        HAL_SPI_DMAStop(hspi);
        HAL_DMA_Abort(hspi->hdmarx);
        HAL_DMA_Abort(hspi->hdmatx);
        HAL_SPI_DeInit(hspi);

        // Wartezeit, um Bus-Fehler zu minimieren
        HAL_Delay(10);

        // SPI und DMA erneut initialisieren
        MX_SPI1_Init();
        Start_SPI_DMA_Transfer();

        printf("✅ SPI & DMA erfolgreich neu gestartet!\n\r");
    }
}
 */






















/*
C-Version
#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 64
__ALIGN_BEGIN uint8_t spi_tx_buffer[TX_BUFFER_SIZE] __ALIGN_END = "STM32_READY";
__ALIGN_BEGIN uint8_t spi_rx_buffer[RX_BUFFER_SIZE] __ALIGN_END;


void Start_SPI_DMA_Transferr() {
    printf("\n🔄 [Start] SPI DMA Transfer wird gestartet...\r\n");

    // SPI DMA Status prüfen
    if (hspi1.State != HAL_SPI_STATE_READY) {
        printf("⚠️ SPI nicht bereit! Aktueller Zustand: %d\r\n", hspi1.State);
        return;
    }

    // Buffer-Inhalt ausgeben (vor dem Transfer)
    printf("📤 TX Buffer vor Übertragung: ");
    for (int i = 0; i < TX_BUFFER_SIZE; i++) {
        printf("%02X ", spi_tx_buffer[i]);
    }
    printf("\r\n");

    printf("📥 RX Buffer vor Übertragung: ");
    for (int i = 0; i < RX_BUFFER_SIZE; i++) {
        printf("%02X ", spi_rx_buffer[i]);
    }
    printf("\r\n");

    // SPI DMA Übertragung starten
    memset(spi_rx_buffer, 0, RX_BUFFER_SIZE);  // RX Buffer vorher leeren
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer, spi_rx_buffer, RX_BUFFER_SIZE);

    if (status != HAL_OK) {
        printf("❌ SPI DMA Start Error! Status Code: %d\r\n", status);

        // Fehlerdiagnose
        if (status == HAL_BUSY) {
            printf("⚠️ SPI ist belegt! Eventuell ist eine vorherige Übertragung noch aktiv.\r\n");
        } else if (status == HAL_ERROR) {
            printf("⚠️ SPI Hardware-Fehler! Eventuell ein Problem mit dem Bus oder DMA.\r\n");
        } else if (status == HAL_TIMEOUT) {
            printf("⚠️ SPI Timeout! Überprüfung der Kommunikation erforderlich.\r\n");
        }

        // SPI-DMA sicher stoppen und neu starten
        HAL_SPI_DMAStop(&hspi1);
        HAL_Delay(10);
        HAL_SPI_DeInit(&hspi1);
        MX_SPI1_Init();
        HAL_Delay(10);

        printf("🔄 SPI & DMA werden erneut gestartet...\r\n");

        // Zweiter Versuch
        memset(spi_rx_buffer, 0, RX_BUFFER_SIZE);  // RX Buffer vorher leeren
        status = HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer, spi_rx_buffer, RX_BUFFER_SIZE);
        if (status != HAL_OK) {
            printf("❌ [Fehler] SPI DMA Restart fehlgeschlagen! System-Reset erforderlich!\r\n");
            Error_Handler();
        } else {
            printf("✅ SPI DMA Transfer erfolgreich nach Neustart!\r\n");
        }
    } else {
        printf("✅ SPI DMA Transfer erfolgreich gestartet!\r\n");
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        printf("\n✅ [SPI Callback] Übertragung abgeschlossen! RX/TX erfolgreich.\r\n");

        // 📥 Debugging: Empfangene RX-Daten als HEX ausgeben
        printf("📥 Empfangene RX-Daten (%d Bytes): ", RX_BUFFER_SIZE);
        for (int i = 0; i < RX_BUFFER_SIZE; i++) {
            printf("%02X ", spi_rx_buffer[i]);
        }
        printf("\r\n");

        // 📥 Empfangene RX-Daten als String
        spi_rx_buffer[RX_BUFFER_SIZE - 1] = '\0';  // Null-Terminierung
        printf("📝 Empfangen als String: \"%s\"\r\n", (char*)spi_rx_buffer);


        // ⚠️ Fehlerprüfung: Ist der Puffer leer?
        if (spi_rx_buffer[0] == 0x00) {
            printf("⚠️ WARNUNG: Empfangene Daten sind leer oder ungültig!\r\n");
        }

        // 📤 Neue TX-Daten für den nächsten Transfer vorbereiten





        index_now++;
        if (index_now >= myarray.size()) {
            index_now = 0;
        }




        snprintf((char*)spi_tx_buffer, TX_BUFFER_SIZE, "DATA TICK: %lu", myarray[index_now]);
        // snprintf((char*)spi_tx_buffer, TX_BUFFER_SIZE, "DATA TICK: %lu", HAL_GetTick());

        // 📤 Debugging: TX-Daten anzeigen
        printf("📤 Gesendete TX-Daten (%d Bytes): \"%s\"\r\n", TX_BUFFER_SIZE, (char*)spi_tx_buffer);

        // 🔄 SPI-DMA RX neu starten, um auf neue Daten zu warten
        printf("🔄 Starte nächste SPI DMA Übertragung...\r\n");
        HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer, spi_rx_buffer, RX_BUFFER_SIZE);
    }
}

*/


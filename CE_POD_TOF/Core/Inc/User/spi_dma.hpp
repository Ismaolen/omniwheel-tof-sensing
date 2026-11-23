/*
 * spi_dma.hpp
 *
 *  Created on: Feb 22, 2025
 *      Author: root
 */

#ifndef INC_USER_SPI_DMA_HPP_
#define INC_USER_SPI_DMA_HPP_



#ifdef __cplusplus
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>  // C++ Header
#include <algorithm>
#include <vector>
#include <memory.h>
#include "../User/tof_sensor.hpp"








extern "C" {
#endif
#define VL53L5CX_1 (0)
#define VL53L5CX_2 (1)
#define VL53L5CX_3 (2)
#define VL53L5CX_4 (3)

    /* SPI Initialisierung und Callbacks */

    //constexpr std::size_t rx_buffer_size = 64;
    //constexpr std::size_t tx_buffer_size = 64;
    //__ALIGN_BEGIN uint8_t spi_tx_buffer[TX_BUFFER_SIZE] __ALIGN_END = "STM32_READY";
    //__ALIGN_BEGIN uint8_t spi_rx_buffer[RX_BUFFER_SIZE] __ALIGN_END;
    // extern uint8_t spi_tx_buffer[TX_BUFFER_SIZE];
    // extern uint8_t spi_rx_buffer[RX_BUFFER_SIZE];
    // andere option static alignas(4) std::array<uint8_t, 64> spi_tx_buffer;
    //static std::array<uint8_t, tx_buffer_size > spi_tx_buffer;
    //static std::array<uint8_t, rx_buffer_size> spi_rx_buffer;


    void Start_SPI_DMA_Transfer_SPI1();
    void Start_SPI_DMA_Transfer_SPI2();
    void prepare_spi1_data();
    void prepare_spi2_data();


#ifdef __cplusplus
}
#endif




#endif /* INC_USER_SPI_DMA_HPP_ */

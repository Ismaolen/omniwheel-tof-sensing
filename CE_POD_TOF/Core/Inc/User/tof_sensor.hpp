/*
 * tof_sensor.hpp
 *
 *  Created on: Jan 5, 2025
 *      Author: root
 */

#ifndef INC_USER_TOF_SENSOR_HPP_
#define INC_USER_TOF_SENSOR_HPP_


#ifdef __cplusplus
#include <iostream>  // C++ Header
#include <algorithm>
#include <vector>
// #include "../../Core/Inc/User/tof_custom_ranging_sensor.hpp"
#include "tof_custom_ranging_sensor.hpp"
#include <cstdint>
#include <array>
#include "tof_sensor_helper.hpp"
// #include "../../Core/Inc/User/tof_sensor_helper.hpp"
inline VL53L5CX_Object_t *pL5obj[CUSTOM_RANGING_INSTANCES_NBR];

inline std::array<bool, 5> is_initialized;

extern "C" {
#include "main.h"
// #include "stm32h5xx_nucleo.h"

#endif

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void VL53L5CX_TOF_Init(void);
void VL53L5CX_TOF_Process(uint32_t instance);
void VL53L5CX_TOF_Prepare(uint32_t instance);
void prepare_tof_sensor();
#ifdef __cplusplus
}
#endif




#endif /* INC_USER_TOF_SENSOR_HPP_ */

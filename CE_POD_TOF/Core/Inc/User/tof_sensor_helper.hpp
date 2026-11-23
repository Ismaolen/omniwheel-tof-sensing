/*
 * tof_sensor_helper.hpp
 *
 *  Created on: Feb 8, 2025
 *      Author: root
 */

#ifndef INC_USER_TOF_SENSOR_HELPER_HPP_
#define INC_USER_TOF_SENSOR_HELPER_HPP_


#ifdef __cplusplus
#include <cstdint>
#include <iostream>

#include "../../Core/Inc/User/tof_custom_ranging_sensor.hpp"
#include <cstdio>
extern "C" {

#endif

enum class LOG_LEVEL {
    // _LEVEL zu den Namen hinzufügen, da INFO und DEBUG als konstanten in
    // andere Dateien deklariert wurden, um konflikte zu vermeiden.
    NONE_LEVEL = 0,
    INFO_LEVEL = 1,
    DEBUG_LEVEL = 2,
    WARN_LEVEL = 3,
    ERROR_LEVEL = 4
};

constexpr auto DEFINED_LOG_LEVEL = LOG_LEVEL::ERROR_LEVEL;

bool VL53L5CX_SimpleRanging_Secure_Init(const uint32_t &instance, const uint16_t &address);

bool Secure_Set_Sensor_Address(const uint32_t &instance, const uint16_t &new_address);


#ifdef __cplusplus
}
#endif




#endif /* INC_USER_TOF_SENSOR_HELPER_HPP_ */

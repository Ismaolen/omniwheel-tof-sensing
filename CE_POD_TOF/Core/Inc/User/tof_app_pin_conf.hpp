/*
 * tof_app_pin_conf.hpp
 *
 *  Created on: Jan 6, 2025
 *      Author: root
 */

#ifndef INC_USER_TOF_APP_PIN_CONF_HPP_
#define INC_USER_TOF_APP_PIN_CONF_HPP_


#include "stm32h5xx_hal.h"

// Externe Variable für Ereigniserkennung
extern volatile uint8_t ToF_EventDetected;

// Definition der TOF-Pins
#define TOF_INT_EXTI_PORT GPIOC
#define TOF_INT_EXTI_PIN GPIO_PIN_0

// Definition der TOF-Pins Sensor 2
#define TOF_INT_EXTI_PORT_2 GPIOC
#define TOF_INT_EXTI_PIN_2 GPIO_PIN_1

// EXTI Callback für STM32G0xx
#ifdef STM32G0xx
inline void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == TOF_INT_EXTI_PIN) {
        ToF_EventDetected = 1;
    }
    if (GPIO_Pin == TOF_INT_EXTI_PIN_2){
        ToF_EventDetected = 1;
    }

}
#else
// EXTI Callback für STM32H5xx
inline void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == TOF_INT_EXTI_PIN) {
        ToF_EventDetected = 1;
    }
    if (GPIO_Pin == TOF_INT_EXTI_PIN_2){
        ToF_EventDetected = 1;
    }
}
#endif



#endif /* INC_USER_TOF_APP_PIN_CONF_HPP_ */

/*
 * tof_usr_conf.hpp
 *
 *  Created on: Jan 6, 2025
 *      Author: root
 */

#ifndef INC_USER_TOF_USR_CONF_HPP_
#define INC_USER_TOF_USR_CONF_HPP_


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

typedef struct
{
    int32_t (*Init)(void *);
    int32_t (*DeInit)(void *);
    int32_t (*ReadID)(void *, uint32_t *);
    int32_t (*GetCapabilities)(void *, void *);
    int32_t (*ConfigProfile)(void *, void *);
    int32_t (*ConfigROI)(void *, void *);
    int32_t (*ConfigIT)(void *, void *);
    int32_t (*GetDistance)(void *, void *);
    int32_t (*Start)(void *, uint8_t);
    int32_t (*Stop)(void *);
    int32_t (*SetAddress)(void *, uint16_t);
    int32_t (*GetAddress)(void *, uint16_t *);
    int32_t (*SetPowerMode)(void *, uint32_t);
    int32_t (*GetPowerMode)(void *, uint32_t *);
} TOF_RANGING_SENSOR_Drv_t;

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
#include "stm32h5xx_nucleo_bus.h"
#include "stm32h5xx_nucleo_errno.h"
#define USE_CUSTOM_RANGING_VL53L5CX (4U)

// CUSTOM_VL53L5CX_1 Sensor 1 Right
#define CUSTOM_VL53L5CX_1_PWR_EN_PORT    GPIOA
#define CUSTOM_VL53L5CX_1_PWR_EN_PIN     GPIO_PIN_8 // D7

#define CUSTOM_VL53L5CX_1_I2C_RST_PORT    GPIOA
#define CUSTOM_VL53L5CX_1_I2C_RST_PIN     GPIO_PIN_0 // A0

#define CUSTOM_VL53L5CX_1_LPn_PORT    GPIOC
#define CUSTOM_VL53L5CX_1_LPn_PIN     GPIO_PIN_9 // D10


// CUSTOM_VL53L5CX_2 Sensor 2 Left
#define CUSTOM_VL53L5CX_2_PWR_EN_PORT    GPIOB
#define CUSTOM_VL53L5CX_2_PWR_EN_PIN     GPIO_PIN_0 // D11

#define CUSTOM_VL53L5CX_2_I2C_RST_PORT    GPIOA
#define CUSTOM_VL53L5CX_2_I2C_RST_PIN     GPIO_PIN_1 // A1

#define CUSTOM_VL53L5CX_2_LPn_PORT    GPIOC
#define CUSTOM_VL53L5CX_2_LPn_PIN     GPIO_PIN_8 // D2



// CUSTOM_VL53L5CX_3 Sensor 3 Left
#define CUSTOM_VL53L5CX_3_PWR_EN_PORT    GPIOB
#define CUSTOM_VL53L5CX_3_PWR_EN_PIN     GPIO_PIN_10 // D6
/// TODO
//#define CUSTOM_VL53L5CX_3_I2C_RST_PORT    GPIOA
//#define CUSTOM_VL53L5CX_3_I2C_RST_PIN     GPIO_PIN_1 // A1

#define CUSTOM_VL53L5CX_3_LPn_PORT    GPIOC
#define CUSTOM_VL53L5CX_3_LPn_PIN     GPIO_PIN_6 // D9


// CUSTOM_VL53L5CX_4 Sensor 4 Left
#define CUSTOM_VL53L5CX_4_PWR_EN_PORT    GPIOB
#define CUSTOM_VL53L5CX_4_PWR_EN_PIN     GPIO_PIN_4 // D5
/// TODO
//#define CUSTOM_VL53L5CX_3_I2C_RST_PORT    GPIOA
//#define CUSTOM_VL53L5CX_3_I2C_RST_PIN     GPIO_PIN_1 // A1

#define CUSTOM_VL53L5CX_4_LPn_PORT    GPIOB
#define CUSTOM_VL53L5CX_4_LPn_PIN     GPIO_PIN_1 // A2











#define CUSTOM_VL53L5CX_I2C_INIT      BSP_I2C1_Init
#define CUSTOM_VL53L5CX_I2C_DEINIT    BSP_I2C1_DeInit
#define CUSTOM_VL53L5CX_I2C_WRITEREG  BSP_I2C1_WriteReg16
#define CUSTOM_VL53L5CX_I2C_READREG   BSP_I2C1_ReadReg16







#ifdef __cplusplus
}
#endif


#endif /* INC_USER_TOF_USR_CONF_HPP_ */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

#include "stm32h5xx_nucleo.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define INT_2_Pin GPIO_PIN_1
#define INT_2_GPIO_Port GPIOC
#define INT_2_EXTI_IRQn EXTI1_IRQn
#define I2C_RCT_1_Pin GPIO_PIN_0
#define I2C_RCT_1_GPIO_Port GPIOA
#define I2C_RCT_2_Pin GPIO_PIN_1
#define I2C_RCT_2_GPIO_Port GPIOA
#define I2C_RCT_3_Pin GPIO_PIN_5
#define I2C_RCT_3_GPIO_Port GPIOC
#define PWR_EN_2_Pin GPIO_PIN_0
#define PWR_EN_2_GPIO_Port GPIOB
#define LPN_4_Pin GPIO_PIN_1
#define LPN_4_GPIO_Port GPIOB
#define PWR_EN_3_Pin GPIO_PIN_10
#define PWR_EN_3_GPIO_Port GPIOB
#define LPN_3_Pin GPIO_PIN_6
#define LPN_3_GPIO_Port GPIOC
#define SPI_CS_Pin GPIO_PIN_7
#define SPI_CS_GPIO_Port GPIOC
#define LPN_2_Pin GPIO_PIN_8
#define LPN_2_GPIO_Port GPIOC
#define LPN_1_Pin GPIO_PIN_9
#define LPN_1_GPIO_Port GPIOC
#define PWR_EN_1_Pin GPIO_PIN_8
#define PWR_EN_1_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define PWR_EN_4_Pin GPIO_PIN_4
#define PWR_EN_4_GPIO_Port GPIOB
#define I2C_RCT_4_Pin GPIO_PIN_5
#define I2C_RCT_4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

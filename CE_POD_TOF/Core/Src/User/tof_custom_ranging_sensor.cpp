/*
 * tof_custom_ranging_sensor.cpp
 *
 *  Created on: Jan 6, 2025
 *      Author: root
 */




/**
  ******************************************************************************
  * @file    custom_ranging_sensor.c
  * @author  IMG SW Application Team
  * @brief   This file provides BSP Ranging Sensors interface for custom boards
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "../../Core/Inc/User/tof_custom_ranging_sensor.hpp"

#include <cstdio>

void *TOF_RANGING_CompObj[CUSTOM_RANGING_INSTANCES_NBR] = {nullptr};
static TOF_RANGING_SENSOR_Drv_t *TOF_RANGING_Drv[CUSTOM_RANGING_INSTANCES_NBR];
static TOF_RANGING_SENSOR_Capabilities_t TOF_RANGING_SENSOR_Cap[CUSTOM_RANGING_INSTANCES_NBR];

static VL53L5CX_Object_t VL53L5CXObjs[CUSTOM_RANGING_INSTANCES_NBR] = {nullptr};  // Für 4 Sensoren

static void reset_device(uint32_t Instance);

#if (USE_CUSTOM_RANGING_VL53L5CX == 4U)
static int32_t TOF_VL53L5CX_Probe(uint32_t Instance, uint16_t Device_Address);
#endif /* Use custom ranging */

/**
  * @brief Initializes the ranging sensor.
  * @param Instance Ranging sensor instance.
  * @param Device_Address The Actual I2C-Address of Ranging sensor instance
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_Init(const uint32_t Instance, uint16_t Device_Address)
{
    int32_t ret;

    // if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else
    {
        reset_device(Instance);

        switch (Instance)
        {
#if (USE_CUSTOM_RANGING_VL53L5CX == 4U)
            case CUSTOM_VL53L5CX_1:
                if (TOF_VL53L5CX_Probe(Instance, Device_Address) != BSP_ERROR_NONE)
                {
                    ret = BSP_ERROR_NO_INIT;
                }
                else
                {
                    ret = BSP_ERROR_NONE;
                }
                break;
            case CUSTOM_VL53L5CX_2:
                if (TOF_VL53L5CX_Probe(Instance, Device_Address) != BSP_ERROR_NONE)
                {
                    ret = BSP_ERROR_NO_INIT;
                }
                else
                {
                    ret = BSP_ERROR_NONE;
                }
                break;

            case CUSTOM_VL53L5CX_3:
                if (TOF_VL53L5CX_Probe(Instance, Device_Address) != BSP_ERROR_NONE)
                {
                    ret = BSP_ERROR_NO_INIT;
                }
                else
                {
                    ret = BSP_ERROR_NONE;
                }
            break;

            case CUSTOM_VL53L5CX_4:
                if (TOF_VL53L5CX_Probe(Instance, Device_Address) != BSP_ERROR_NONE)
                {
                    ret = BSP_ERROR_NO_INIT;
                }
                else
                {
                    ret = BSP_ERROR_NONE;
                }
            break;
#endif /* Use custom ranging */


            default:
                ret = BSP_ERROR_UNKNOWN_COMPONENT;
                break;
        }
    }

    return ret;
}

/**
  * @brief Deinitializes the ranging sensor.
  * @param Instance    Ranging sensor instance.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_DeInit(uint32_t Instance)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->DeInit(TOF_RANGING_CompObj[Instance]) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}

/**
  * @brief Read the ranging sensor device ID.
  * @param Instance    Ranging sensor instance.
  * @param pId    Pointer to the device ID.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_ReadID(uint32_t Instance, uint32_t *pId)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->ReadID(TOF_RANGING_CompObj[Instance], pId) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}

/**
  * @brief Get the ranging sensor capabilities.
  * @param Instance    Ranging sensor instance.
  * @param pCapabilities    Pointer to the ranging sensor capabilities.
  * @note This function should be called after the init.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_GetCapabilities(uint32_t Instance, TOF_RANGING_SENSOR_Capabilities_t *pCapabilities)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->GetCapabilities(TOF_RANGING_CompObj[Instance], pCapabilities) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}

/**
  * @brief Set the ranging configuration profile.
  * @param Instance    Ranging sensor instance.
  * @param pConfig    Pointer to the new configuration profile to be applied.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_ConfigProfile(uint32_t Instance, RANGING_SENSOR_ProfileConfig_t *pConfig)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->ConfigProfile(TOF_RANGING_CompObj[Instance], pConfig) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}


/**
  * @brief Start ranging.
  * @param Instance    Ranging sensor instance.
  * @param Mode        The desired ranging mode.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_Start(uint32_t Instance, uint8_t Mode)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->Start(TOF_RANGING_CompObj[Instance], Mode) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}

/**
  * @brief Stop ranging.
  * @param Instance    Ranging sensor instance.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_Stop(uint32_t Instance)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->Stop(TOF_RANGING_CompObj[Instance]) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}

/**
  * @brief Set The I2C address of the device.
  * @param Instance    Ranging sensor instance.
  * @param Address     New I2C address.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_SetAddress(uint32_t Instance, uint16_t Address)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {

        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->SetAddress(TOF_RANGING_CompObj[Instance], Address) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}

/**
  * @brief Get the I2C address of the device.
  * @param Instance    Ranging sensor instance.
  * @param pAddress    Pointer to the current I2C address.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_GetAddress(uint32_t Instance, uint16_t *pAddress)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->GetAddress(TOF_RANGING_CompObj[Instance], pAddress) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}



#if (USE_CUSTOM_RANGING_VL53L5CX == 4U)
/**
  * @brief Register Bus IOs if component ID is OK.
  * @param Instance    Ranging sensor instance.
  * @retval BSP status
  */
static int32_t TOF_VL53L5CX_Probe(uint32_t Instance, uint16_t Device_Address)
{
    int32_t ret;
    VL53L5CX_IO_t              IOCtx;
    uint32_t                   id;
    // static VL53L5CX_Object_t   VL53L5CXObj;
    VL53L5CX_Object_t *pObj  = &VL53L5CXObjs[Instance];

    /* Configure the ranging sensor driver */
    // IOCtx.Address     = RANGING_SENSOR_VL53L5CX_ADDRESS;
    IOCtx.Address     = Device_Address;
    IOCtx.Init        = CUSTOM_VL53L5CX_I2C_INIT;
    IOCtx.DeInit      = CUSTOM_VL53L5CX_I2C_DEINIT;
    IOCtx.WriteReg    = CUSTOM_VL53L5CX_I2C_WRITEREG;
    IOCtx.ReadReg     = CUSTOM_VL53L5CX_I2C_READREG;
    IOCtx.GetTick     = BSP_GetTick;

    // if (VL53L5CX_RegisterBusIO(&VL53L5CXObj, &IOCtx) != VL53L5CX_OK)
    if (VL53L5CX_RegisterBusIO(pObj, &IOCtx) != VL53L5CX_OK)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        TOF_RANGING_Drv[Instance] = reinterpret_cast<TOF_RANGING_SENSOR_Drv_t *>(&VL53L5CX_RANGING_SENSOR_Driver);
        TOF_RANGING_CompObj[Instance] = pObj;
        //if(Instance == CUSTOM_VL53L5CX_2) {
        //    VL53L5CXObj.IsInitialized = 0;
        //}
        // if (VL53L5CX_ReadID(&VL53L5CXObj, &id) != VL53L5CX_OK)
        if (VL53L5CX_ReadID(pObj, &id) != VL53L5CX_OK)
        {
            ret = BSP_ERROR_COMPONENT_FAILURE;
            printf("\n\nret = %d\t [FILE:%s Function:%s LINE:%d]\n\r", ret, __FILE__, __PRETTY_FUNCTION__, __LINE__);
        }
        else if (id != VL53L5CX_ID)
        {
            ret = BSP_ERROR_UNKNOWN_COMPONENT;
        }

        else if (TOF_RANGING_Drv[Instance]->Init(TOF_RANGING_CompObj[Instance]) != VL53L5CX_OK)
        {
            ret = BSP_ERROR_COMPONENT_FAILURE;
            printf("\n\nret = %d\t [FILE:%s Function:%s LINE:%d]\n\r", ret, __FILE__, __PRETTY_FUNCTION__, __LINE__);
        }



        else if (TOF_RANGING_Drv[Instance]->GetCapabilities(TOF_RANGING_CompObj[Instance],
                                                               &TOF_RANGING_SENSOR_Cap[Instance]) != VL53L5CX_OK)
        {
            ret = BSP_ERROR_COMPONENT_FAILURE;
            printf("\n\nret = %d\t [FILE:%s Function:%s LINE:%d]\n\r", ret, __FILE__, __PRETTY_FUNCTION__, __LINE__);

        }
        else
        {
            ret = BSP_ERROR_NONE;
        }
    }
    printf("\n\nret = %d\t [FILE:%s Function:%s LINE:%d]\n\r", ret, __FILE__, __PRETTY_FUNCTION__, __LINE__);
    return ret;
}
#endif /* Use custom ranging */

static void reset_device(uint32_t Instance)
{
#if (USE_CUSTOM_RANGING_VL53L5CX == 4U)
    if (Instance == CUSTOM_VL53L5CX_1) {
        //HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_PWR_EN_PORT, CUSTOM_VL53L5CX_1_PWR_EN_PIN, GPIO_PIN_RESET);
        //HAL_Delay(2);
        //HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_PWR_EN_PORT, CUSTOM_VL53L5CX_1_PWR_EN_PIN, GPIO_PIN_SET);
        //HAL_Delay(2);
        HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_LPn_PORT, CUSTOM_VL53L5CX_1_LPn_PIN, GPIO_PIN_RESET);
        HAL_Delay(2);
        HAL_GPIO_WritePin(CUSTOM_VL53L5CX_1_LPn_PORT, CUSTOM_VL53L5CX_1_LPn_PIN, GPIO_PIN_SET);
        HAL_Delay(2);
    }
    else if (Instance == CUSTOM_VL53L5CX_2) {
        //HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_PWR_EN_PORT, CUSTOM_VL53L5CX_2_PWR_EN_PIN, GPIO_PIN_RESET);
        HAL_Delay(2);
        //HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_PWR_EN_PORT, CUSTOM_VL53L5CX_2_PWR_EN_PIN, GPIO_PIN_SET);
        HAL_Delay(2);
        HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_LPn_PORT, CUSTOM_VL53L5CX_2_LPn_PIN, GPIO_PIN_RESET);
        HAL_Delay(2);
        HAL_GPIO_WritePin(CUSTOM_VL53L5CX_2_LPn_PORT, CUSTOM_VL53L5CX_2_LPn_PIN, GPIO_PIN_SET);
        HAL_Delay(2);
    }

    else if (Instance == CUSTOM_VL53L5CX_3) {
        //HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_PWR_EN_PORT, CUSTOM_VL53L5CX_3_PWR_EN_PIN, GPIO_PIN_RESET);
        HAL_Delay(2);
        //HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_PWR_EN_PORT, CUSTOM_VL53L5CX_3_PWR_EN_PIN, GPIO_PIN_SET);
        HAL_Delay(2);
        HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_LPn_PORT, CUSTOM_VL53L5CX_3_LPn_PIN, GPIO_PIN_RESET);
        HAL_Delay(2);
        HAL_GPIO_WritePin(CUSTOM_VL53L5CX_3_LPn_PORT, CUSTOM_VL53L5CX_3_LPn_PIN, GPIO_PIN_SET);
        HAL_Delay(2);
    }

    else if (Instance == CUSTOM_VL53L5CX_4) {
        //HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_PWR_EN_PORT, CUSTOM_VL53L5CX_4_PWR_EN_PIN, GPIO_PIN_RESET);
        HAL_Delay(2);
        //HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_PWR_EN_PORT, CUSTOM_VL53L5CX_4_PWR_EN_PIN, GPIO_PIN_SET);
        HAL_Delay(2);
        HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_LPn_PORT, CUSTOM_VL53L5CX_4_LPn_PIN, GPIO_PIN_RESET);
        HAL_Delay(2);
        HAL_GPIO_WritePin(CUSTOM_VL53L5CX_4_LPn_PORT, CUSTOM_VL53L5CX_4_LPn_PIN, GPIO_PIN_SET);
        HAL_Delay(2);
    }

#endif /* Use custom ranging */
}












/**
  * @brief Set the power mode.
  * @param Instance    Ranging sensor instance.
  * @param PowerMode    New power mode to be entered.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_SetPowerMode(uint32_t Instance, uint32_t PowerMode)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->SetPowerMode(TOF_RANGING_CompObj[Instance], PowerMode) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}


/**
  * @brief Get the power mode.
  * @param Instance    Ranging sensor instance.
  * @param pPowerMode    Pointer to the current power mode.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_GetPowerMode(uint32_t Instance, uint32_t *pPowerMode)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->GetPowerMode(TOF_RANGING_CompObj[Instance], pPowerMode) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}




/**
  * @brief Configure the Region of Interest of the ranging sensor.
  * @param Instance    Ranging sensor instance.
  * @param pConfig    Pointer to the ROI configuration struct.
  * @note Should be called only if the device supports CustomROI.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_ConfigROI(uint32_t Instance, RANGING_SENSOR_ROIConfig_t *pConfig)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_SENSOR_Cap[Instance].CustomROI == 0)
    {
        ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    }
    else if (TOF_RANGING_Drv[Instance]->ConfigROI(TOF_RANGING_CompObj[Instance], pConfig) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}

/**
  * @brief Configure the IT event generation parameters.
  * @param Instance    Ranging sensor instance.
  * @param pConfig    Pointer to the IT configuration struct.
  * @note The threshold modes can be used only if supported by the device (check the capabilities)
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_ConfigIT(uint32_t Instance, RANGING_SENSOR_ITConfig_t *pConfig)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->ConfigIT(TOF_RANGING_CompObj[Instance], pConfig) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}





/**
  * @brief Get the last distance measurement information.
  * @param Instance    Ranging sensor instance.
  * @param pResult    Pointer to the result struct.
  * @retval BSP status
  */
int32_t TOF_RANGING_SENSOR_GetDistance(uint32_t Instance, RANGING_SENSOR_Result_t *pResult)
{
    int32_t ret;

    if (Instance >= CUSTOM_RANGING_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (TOF_RANGING_Drv[Instance]->GetDistance(TOF_RANGING_CompObj[Instance], pResult) < 0)
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}
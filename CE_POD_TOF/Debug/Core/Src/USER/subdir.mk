################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/USER/tof_sensor.c 

OBJS += \
./Core/Src/USER/tof_sensor.o 

C_DEPS += \
./Core/Src/USER/tof_sensor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/USER/%.o Core/Src/USER/%.su Core/Src/USER/%.cyclo: ../Core/Src/USER/%.c Core/Src/USER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H533xx -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Components/vl53l5cx/modules -I../Drivers/BSP/Components/vl53l5cx/porting -I../Drivers/BSP/Components/vl53l5cx -I../TOF/App -I../TOF/Target -I../Drivers/BSP/STM32H5xx_Nucleo -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-USER

clean-Core-2f-Src-2f-USER:
	-$(RM) ./Core/Src/USER/tof_sensor.cyclo ./Core/Src/USER/tof_sensor.d ./Core/Src/USER/tof_sensor.o ./Core/Src/USER/tof_sensor.su

.PHONY: clean-Core-2f-Src-2f-USER


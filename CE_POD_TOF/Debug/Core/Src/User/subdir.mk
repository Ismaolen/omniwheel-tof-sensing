################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/User/spi_dma.cpp \
../Core/Src/User/tof_custom_ranging_sensor.cpp \
../Core/Src/User/tof_sensor.cpp \
../Core/Src/User/tof_sensor_helper.cpp 

OBJS += \
./Core/Src/User/spi_dma.o \
./Core/Src/User/tof_custom_ranging_sensor.o \
./Core/Src/User/tof_sensor.o \
./Core/Src/User/tof_sensor_helper.o 

CPP_DEPS += \
./Core/Src/User/spi_dma.d \
./Core/Src/User/tof_custom_ranging_sensor.d \
./Core/Src/User/tof_sensor.d \
./Core/Src/User/tof_sensor_helper.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/User/%.o Core/Src/User/%.su Core/Src/User/%.cyclo: ../Core/Src/User/%.cpp Core/Src/User/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H533xx -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Components/vl53l5cx/modules -I../Drivers/BSP/Components/vl53l5cx/porting -I../Drivers/BSP/Components/vl53l5cx -I../TOF/App -I../TOF/Target -I../Drivers/BSP/STM32H5xx_Nucleo -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -std=gnu++23 -o "$@"

clean: clean-Core-2f-Src-2f-User

clean-Core-2f-Src-2f-User:
	-$(RM) ./Core/Src/User/spi_dma.cyclo ./Core/Src/User/spi_dma.d ./Core/Src/User/spi_dma.o ./Core/Src/User/spi_dma.su ./Core/Src/User/tof_custom_ranging_sensor.cyclo ./Core/Src/User/tof_custom_ranging_sensor.d ./Core/Src/User/tof_custom_ranging_sensor.o ./Core/Src/User/tof_custom_ranging_sensor.su ./Core/Src/User/tof_sensor.cyclo ./Core/Src/User/tof_sensor.d ./Core/Src/User/tof_sensor.o ./Core/Src/User/tof_sensor.su ./Core/Src/User/tof_sensor_helper.cyclo ./Core/Src/User/tof_sensor_helper.d ./Core/Src/User/tof_sensor_helper.o ./Core/Src/User/tof_sensor_helper.su

.PHONY: clean-Core-2f-Src-2f-User


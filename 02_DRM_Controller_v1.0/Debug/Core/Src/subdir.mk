################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/display.c \
../Core/Src/freertos.c \
../Core/Src/logic.c \
../Core/Src/main.c \
../Core/Src/peripherals.c \
../Core/Src/stm32l0xx_hal_msp.c \
../Core/Src/stm32l0xx_it.c \
../Core/Src/system_stm32l0xx.c \
../Core/Src/uart.c 

OBJS += \
./Core/Src/display.o \
./Core/Src/freertos.o \
./Core/Src/logic.o \
./Core/Src/main.o \
./Core/Src/peripherals.o \
./Core/Src/stm32l0xx_hal_msp.o \
./Core/Src/stm32l0xx_it.o \
./Core/Src/system_stm32l0xx.o \
./Core/Src/uart.o 

C_DEPS += \
./Core/Src/display.d \
./Core/Src/freertos.d \
./Core/Src/logic.d \
./Core/Src/main.d \
./Core/Src/peripherals.d \
./Core/Src/stm32l0xx_hal_msp.d \
./Core/Src/stm32l0xx_it.d \
./Core/Src/system_stm32l0xx.d \
./Core/Src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32L071xx -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0/Core/Inc" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0/Drivers/STM32L0xx_HAL_Driver/Inc" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0/Middlewares/Third_Party/FreeRTOS/Source/include" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../SW4STM32/startup_stm32l071xx.s 

C_SRCS += \
../SW4STM32/syscalls.c 

OBJS += \
./SW4STM32/startup_stm32l071xx.o \
./SW4STM32/syscalls.o 

C_DEPS += \
./SW4STM32/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
SW4STM32/%.o: ../SW4STM32/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SW4STM32/%.o: ../SW4STM32/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32L071xx -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0_magnetic_Slave/Core/Inc" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0_magnetic_Slave/Drivers/STM32L0xx_HAL_Driver/Inc" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0_magnetic_Slave/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0_magnetic_Slave/Middlewares/Third_Party/FreeRTOS/Source/include" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0_magnetic_Slave/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0_magnetic_Slave/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0_magnetic_Slave/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"D:/이준우/02_softwave/2025_magnetic Sensor/02_DRM_Controller_v1.0_magnetic_Slave/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



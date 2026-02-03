################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/bsp_driver_sd.c \
../Src/display.c \
../Src/ethernetif.c \
../Src/fatfs.c \
../Src/fatfs_platform.c \
../Src/flash_if.c \
../Src/freertos.c \
../Src/function.c \
../Src/image.c \
../Src/logic.c \
../Src/lwip.c \
../Src/main.c \
../Src/peripherals.c \
../Src/sd_diskio.c \
../Src/sdcard.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f4xx.c \
../Src/uart.c 

OBJS += \
./Src/bsp_driver_sd.o \
./Src/display.o \
./Src/ethernetif.o \
./Src/fatfs.o \
./Src/fatfs_platform.o \
./Src/flash_if.o \
./Src/freertos.o \
./Src/function.o \
./Src/image.o \
./Src/logic.o \
./Src/lwip.o \
./Src/main.o \
./Src/peripherals.o \
./Src/sd_diskio.o \
./Src/sdcard.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f4xx.o \
./Src/uart.o 

C_DEPS += \
./Src/bsp_driver_sd.d \
./Src/display.d \
./Src/ethernetif.d \
./Src/fatfs.d \
./Src/fatfs_platform.d \
./Src/flash_if.d \
./Src/freertos.d \
./Src/function.d \
./Src/image.d \
./Src/logic.d \
./Src/lwip.d \
./Src/main.d \
./Src/peripherals.d \
./Src/sd_diskio.d \
./Src/sdcard.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f4xx.d \
./Src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F427xx -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Inc" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/FatFs/src" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/CMSIS/Include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/system" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/netif/ppp" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/lwip" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/lwip/apps" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/lwip/priv" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/lwip/prot" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/netif" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/posix" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/posix/sys" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/system/arch" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/CMSIS/Include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.c 

OBJS += \
./Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.o 

C_DEPS += \
./Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/LwIP/src/core/ipv6/%.o: ../Middlewares/Third_Party/LwIP/src/core/ipv6/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F427xx -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Inc" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/FatFs/src" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/CMSIS/Include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/system" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/netif/ppp" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/lwip" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/lwip/apps" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/lwip/priv" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/lwip/prot" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/netif" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/posix" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/src/include/posix/sys" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Middlewares/Third_Party/LwIP/system/arch" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/CMSIS/Include" -I"C:/2018/201810_EX_TK_HQ/02_DisplayController_v1.1/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



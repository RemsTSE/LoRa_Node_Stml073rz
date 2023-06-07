################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/LoRa.c \
../Core/Src/main.c \
../Core/Src/routing_table.c \
../Core/Src/scheduler.c \
../Core/Src/stm32l0xx_hal_msp.c \
../Core/Src/stm32l0xx_hal_uart.c \
../Core/Src/stm32l0xx_hal_uart_ex.c \
../Core/Src/stm32l0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l0xx.c 

C_DEPS += \
./Core/Src/LoRa.d \
./Core/Src/main.d \
./Core/Src/routing_table.d \
./Core/Src/scheduler.d \
./Core/Src/stm32l0xx_hal_msp.d \
./Core/Src/stm32l0xx_hal_uart.d \
./Core/Src/stm32l0xx_hal_uart_ex.d \
./Core/Src/stm32l0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l0xx.d 

OBJS += \
./Core/Src/LoRa.o \
./Core/Src/main.o \
./Core/Src/routing_table.o \
./Core/Src/scheduler.o \
./Core/Src/stm32l0xx_hal_msp.o \
./Core/Src/stm32l0xx_hal_uart.o \
./Core/Src/stm32l0xx_hal_uart_ex.o \
./Core/Src/stm32l0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l0xx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Drivers/CMSIS/Include -I"C:/Users/remil/STM32CubeIDE/workspace_1.10.1/LORA_node/Drivers/OLED" -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/LoRa.cyclo ./Core/Src/LoRa.d ./Core/Src/LoRa.o ./Core/Src/LoRa.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/routing_table.cyclo ./Core/Src/routing_table.d ./Core/Src/routing_table.o ./Core/Src/routing_table.su ./Core/Src/scheduler.cyclo ./Core/Src/scheduler.d ./Core/Src/scheduler.o ./Core/Src/scheduler.su ./Core/Src/stm32l0xx_hal_msp.cyclo ./Core/Src/stm32l0xx_hal_msp.d ./Core/Src/stm32l0xx_hal_msp.o ./Core/Src/stm32l0xx_hal_msp.su ./Core/Src/stm32l0xx_hal_uart.cyclo ./Core/Src/stm32l0xx_hal_uart.d ./Core/Src/stm32l0xx_hal_uart.o ./Core/Src/stm32l0xx_hal_uart.su ./Core/Src/stm32l0xx_hal_uart_ex.cyclo ./Core/Src/stm32l0xx_hal_uart_ex.d ./Core/Src/stm32l0xx_hal_uart_ex.o ./Core/Src/stm32l0xx_hal_uart_ex.su ./Core/Src/stm32l0xx_it.cyclo ./Core/Src/stm32l0xx_it.d ./Core/Src/stm32l0xx_it.o ./Core/Src/stm32l0xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l0xx.cyclo ./Core/Src/system_stm32l0xx.d ./Core/Src/system_stm32l0xx.o ./Core/Src/system_stm32l0xx.su

.PHONY: clean-Core-2f-Src


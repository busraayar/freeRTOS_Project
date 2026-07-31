################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/can.c \
../Core/Src/core_mqtt_agent.c \
../Core/Src/core_mqtt_agent_command_functions.c \
../Core/Src/eth.c \
../Core/Src/gpio.c \
../Core/Src/main.c \
../Core/Src/rng.c \
../Core/Src/rtos.c \
../Core/Src/stm32f7xx_hal_msp.c \
../Core/Src/stm32f7xx_hal_timebase_tim.c \
../Core/Src/stm32f7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/system_stm32f7xx.c \
../Core/Src/tim.c 

OBJS += \
./Core/Src/can.o \
./Core/Src/core_mqtt_agent.o \
./Core/Src/core_mqtt_agent_command_functions.o \
./Core/Src/eth.o \
./Core/Src/gpio.o \
./Core/Src/main.o \
./Core/Src/rng.o \
./Core/Src/rtos.o \
./Core/Src/stm32f7xx_hal_msp.o \
./Core/Src/stm32f7xx_hal_timebase_tim.o \
./Core/Src/stm32f7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/system_stm32f7xx.o \
./Core/Src/tim.o 

C_DEPS += \
./Core/Src/can.d \
./Core/Src/core_mqtt_agent.d \
./Core/Src/core_mqtt_agent_command_functions.d \
./Core/Src/eth.d \
./Core/Src/gpio.d \
./Core/Src/main.d \
./Core/Src/rng.d \
./Core/Src/rtos.d \
./Core/Src/stm32f7xx_hal_msp.d \
./Core/Src/stm32f7xx_hal_timebase_tim.d \
./Core/Src/stm32f7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/system_stm32f7xx.d \
./Core/Src/tim.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DSTM32F7xx -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/portable/GCC/ARM_CM7/r0p1" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/FreeRTOS-Plus-TCP/source/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/STM32/Drivers/F7" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/coreMQTT/source/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/coreMQTT/source/include/private" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/coreMQTT/source/interface" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/can.cyclo ./Core/Src/can.d ./Core/Src/can.o ./Core/Src/can.su ./Core/Src/core_mqtt_agent.cyclo ./Core/Src/core_mqtt_agent.d ./Core/Src/core_mqtt_agent.o ./Core/Src/core_mqtt_agent.su ./Core/Src/core_mqtt_agent_command_functions.cyclo ./Core/Src/core_mqtt_agent_command_functions.d ./Core/Src/core_mqtt_agent_command_functions.o ./Core/Src/core_mqtt_agent_command_functions.su ./Core/Src/eth.cyclo ./Core/Src/eth.d ./Core/Src/eth.o ./Core/Src/eth.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/rng.cyclo ./Core/Src/rng.d ./Core/Src/rng.o ./Core/Src/rng.su ./Core/Src/rtos.cyclo ./Core/Src/rtos.d ./Core/Src/rtos.o ./Core/Src/rtos.su ./Core/Src/stm32f7xx_hal_msp.cyclo ./Core/Src/stm32f7xx_hal_msp.d ./Core/Src/stm32f7xx_hal_msp.o ./Core/Src/stm32f7xx_hal_msp.su ./Core/Src/stm32f7xx_hal_timebase_tim.cyclo ./Core/Src/stm32f7xx_hal_timebase_tim.d ./Core/Src/stm32f7xx_hal_timebase_tim.o ./Core/Src/stm32f7xx_hal_timebase_tim.su ./Core/Src/stm32f7xx_it.cyclo ./Core/Src/stm32f7xx_it.d ./Core/Src/stm32f7xx_it.o ./Core/Src/stm32f7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/system_stm32f7xx.cyclo ./Core/Src/system_stm32f7xx.d ./Core/Src/system_stm32f7xx.o ./Core/Src/system_stm32f7xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su

.PHONY: clean-Core-2f-Src


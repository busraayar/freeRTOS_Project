################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/stm32f7xx_hal_eth.c 

OBJS += \
./ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/stm32f7xx_hal_eth.o 

C_DEPS += \
./ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/stm32f7xx_hal_eth.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/%.o ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/%.su ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/%.cyclo: ../ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/%.c ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g -DDEBUG -DSTM32F7xx -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/portable/GCC/ARM_CM7/r0p1" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS-Plus-TCP" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS-Plus-TCP/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS-Plus-TCP/portable/Compiler/GCC" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS-Plus-TCP/portable/BufferManagement" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ThirdParty-2f-FreeRTOS-2d-Plus-2d-TCP-2f-portable-2f-NetworkInterface-2f-STM32-2f-Drivers-2f-F7

clean-ThirdParty-2f-FreeRTOS-2d-Plus-2d-TCP-2f-portable-2f-NetworkInterface-2f-STM32-2f-Drivers-2f-F7:
	-$(RM) ./ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/stm32f7xx_hal_eth.cyclo ./ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/stm32f7xx_hal_eth.d ./ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/stm32f7xx_hal_eth.o ./ThirdParty/FreeRTOS-Plus-TCP/portable/NetworkInterface/STM32/Drivers/F7/stm32f7xx_hal_eth.su

.PHONY: clean-ThirdParty-2f-FreeRTOS-2d-Plus-2d-TCP-2f-portable-2f-NetworkInterface-2f-STM32-2f-Drivers-2f-F7


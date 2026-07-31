################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f767zitx.s 

OBJS += \
./Core/Startup/startup_stm32f767zitx.o 

S_DEPS += \
./Core/Startup/startup_stm32f767zitx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -DDEBUG -DSTM32F7xx -DSTM32F767xx -c -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/portable/GCC/ARM_CM7/r0p1" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/Core/Inc" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/Core/Src/tasks/AppTask" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/Core/Src/tasks/CanTask" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/Core/Src/tasks/MqttTask" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/FreeRTOS-Plus-TCP/source/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/STM32/Drivers/F7" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/coreMQTT/source/include" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/coreMQTT/source/include/private" -I"C:/workspace/PersonalStudy/freeRTOS/FreeRTOS_Projects/ThirdParty/FreeRTOS/FreeRTOS-Plus/FreeRTOS-Plus-TCP/Source/coreMQTT/source/interface" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f767zitx.d ./Core/Startup/startup_stm32f767zitx.o

.PHONY: clean-Core-2f-Startup


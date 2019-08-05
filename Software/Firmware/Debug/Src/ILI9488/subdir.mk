################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/ILI9488/ILI9488_GFX.c \
../Src/ILI9488/ILI9488_STM32_Driver.c 

OBJS += \
./Src/ILI9488/ILI9488_GFX.o \
./Src/ILI9488/ILI9488_STM32_Driver.o 

C_DEPS += \
./Src/ILI9488/ILI9488_GFX.d \
./Src/ILI9488/ILI9488_STM32_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
Src/ILI9488/ILI9488_GFX.o: ../Src/ILI9488/ILI9488_GFX.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/ILI9488/ILI9488_GFX.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/ILI9488/ILI9488_STM32_Driver.o: ../Src/ILI9488/ILI9488_STM32_Driver.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/ILI9488/ILI9488_STM32_Driver.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"


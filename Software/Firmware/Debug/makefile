################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/ILI9488/subdir.mk
-include Src/subdir.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
AmplifierTDA7439.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
AmplifierTDA7439.list \


# All Target
all: AmplifierTDA7439.elf secondary-outputs

# Tool invocations
AmplifierTDA7439.elf: $(OBJS) $(USER_OBJS) D:\Projects\AmplifierTDA7439\AmplifierTDA7439\Software\Firmware\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "AmplifierTDA7439.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\Projects\AmplifierTDA7439\AmplifierTDA7439\Software\Firmware\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="AmplifierTDA7439.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

AmplifierTDA7439.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "AmplifierTDA7439.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

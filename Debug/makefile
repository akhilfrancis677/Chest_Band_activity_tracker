################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include src/tasks/subdir.mk
-include src/bluetooth/Proxi/subdir.mk
-include src/bluetooth/Common/utilities/subdir.mk
-include src/bluetooth/Common/tl/subdir.mk
-include src/bluetooth/Common/hw_interface/subdir.mk
-include src/bluetooth/Common/ble_services/subdir.mk
-include src/bluetooth/Common/ble_core/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/wm8994/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/stmpe811/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/stmpe1600/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/st7789h2/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/st7735/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/ov9655/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/mfxstm32l152/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/m24sr/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/lsm6dsl/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/lsm303dlhc/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/lsm303c/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/ls016b8uy/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/lps22hb/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/lis3mdl/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/l3gd20/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/hx8347i/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/hx8347g/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/hts221/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/ft6x06/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/ft5336/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/ft3x67/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/Components/cs42l51/subdir.mk
-include src/L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/subdir.mk
-include src/Applications/subdir.mk
-include HAL_Driver/Src/Legacy/subdir.mk
-include HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Activity_Tracker.elf

# Tool invocations
Activity_Tracker.elf: $(OBJS) $(USER_OBJS) C:\Users\afrancis\Downloads\Chest_Band_activity_tracker/LinkerScript.ld
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -T"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/LinkerScript.ld" -Wl,-Map=output.map -Wl,--gc-sections -o "Activity_Tracker.elf" @"objects.list" $(USER_OBJS) $(LIBS) -lm
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "Activity_Tracker.elf" "Activity_Tracker.bin"
	arm-none-eabi-size "Activity_Tracker.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

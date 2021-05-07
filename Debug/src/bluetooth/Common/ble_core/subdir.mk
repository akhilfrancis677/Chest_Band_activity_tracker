################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bluetooth/Common/ble_core/bluenrg_gap_aci.c \
../src/bluetooth/Common/ble_core/bluenrg_gatt_aci.c \
../src/bluetooth/Common/ble_core/bluenrg_hal_aci.c \
../src/bluetooth/Common/ble_core/osal.c 

OBJS += \
./src/bluetooth/Common/ble_core/bluenrg_gap_aci.o \
./src/bluetooth/Common/ble_core/bluenrg_gatt_aci.o \
./src/bluetooth/Common/ble_core/bluenrg_hal_aci.o \
./src/bluetooth/Common/ble_core/osal.o 

C_DEPS += \
./src/bluetooth/Common/ble_core/bluenrg_gap_aci.d \
./src/bluetooth/Common/ble_core/bluenrg_gatt_aci.d \
./src/bluetooth/Common/ble_core/bluenrg_hal_aci.d \
./src/bluetooth/Common/ble_core/osal.d 


# Each subdirectory must supply rules for building sources it contributes
src/bluetooth/Common/ble_core/%.o: ../src/bluetooth/Common/ble_core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32L4 -DSTM32L475VGTx -DDEBUG -DSTM32L475xx -DUSE_HAL_DRIVER -DBLUENRG_MS -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/HAL_Driver/Inc/Legacy" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/inc" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/CMSIS/device" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/CMSIS/core" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/HAL_Driver/Inc" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/include/includes/ble_core" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/include/includes/ble_services" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/include/includes/hr_rate" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/include/includes/hw" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/include/includes/tl" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/include/includes/utilities" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/include/includes" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/src/L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/src/L4_IOT_Sensors/Drivers/BSP/Components/hts221" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/src/Applications}" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/src/tasks" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/src/Drivers" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/Project/src/tasks" -I"C:/Users/afrancis/Downloads/Chest_Band_activity_tracker/Project/src/Drivers" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



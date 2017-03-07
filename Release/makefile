################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include example/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: periph_uart.axf

# Tool invocations
periph_uart.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Thomas\Documents\LPCXpresso_8.2.2_650\workspace\lpc_chip_18xx\Release" -L"C:\Users\Thomas\Documents\LPCXpresso_8.2.2_650\workspace\lpc_board_keil_mcb_1857\Release" -Xlinker -Map="periph_uart.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "periph_uart_Release.ld" -o "periph_uart.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) periph_uart.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "periph_uart.axf"; # arm-none-eabi-objcopy -v -O binary "periph_uart.axf" "periph_uart.bin" ; # checksum -p LPC1857 -d "periph_uart.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

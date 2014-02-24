################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/C6000 Code Generation Tools 7.4.6

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./src/platform.obj" \
"./src/evmc66x_uart.obj" \
"./src/evmc66x_spi.obj" \
"./src/evmc66x_nor.obj" \
"./src/evmc66x_i2c_eeprom.obj" \
"./src/evmc66x_gpio.obj" \
"./src/evmc66x_fpga.obj" \
"./src/evmc66x_ecc.obj" \
"./src/evmc6670_phy.obj" \
"./src/evmc6670.obj" \
"./src/evm66x_nand.obj" \
"./src/evm66x_i2c.obj" \
"../lib/debug/ti.platform.evm6670l.lite.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir_vars.mk
-include lib/debug/subdir_vars.mk
-include src/subdir_rules.mk
-include lib/debug/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: C:/Users/mahmoud/workspace_v5_5/platform_lib_evmc6670l/lib/release/ti.platform.evm6670l.ae66e

# Tool invocations
C:/Users/mahmoud/workspace_v5_5/platform_lib_evmc6670l/lib/release/ti.platform.evm6670l.ae66e: $(OBJS) $(OBJ_SRCS) $(LIB_SRCS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Archiver'
	"C:/ti/C6000 Code Generation Tools 7.4.6/bin/ar6x" r "C:/Users/mahmoud/workspace_v5_5/platform_lib_evmc6670l/lib/release/ti.platform.evm6670l.ae66e" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C6000_LIBRARY_OUTPUTS__QUOTED) "C:\Users\mahmoud\workspace_v5_5\platform_lib_evmc6670l\lib\release\ti.platform.evm6670l.ae66e"
	-$(RM) "src\evm66x_i2c.pp" "src\evm66x_nand.pp" "src\evmc6670.pp" "src\evmc6670_phy.pp" "src\evmc66x_ecc.pp" "src\evmc66x_fpga.pp" "src\evmc66x_gpio.pp" "src\evmc66x_i2c_eeprom.pp" "src\evmc66x_nor.pp" "src\evmc66x_spi.pp" "src\evmc66x_uart.pp" "src\platform.pp" 
	-$(RM) "src\evm66x_i2c.obj" "src\evm66x_nand.obj" "src\evmc6670.obj" "src\evmc6670_phy.obj" "src\evmc66x_ecc.obj" "src\evmc66x_fpga.obj" "src\evmc66x_gpio.obj" "src\evmc66x_i2c_eeprom.obj" "src\evmc66x_nor.obj" "src\evmc66x_spi.obj" "src\evmc66x_uart.obj" "src\platform.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets


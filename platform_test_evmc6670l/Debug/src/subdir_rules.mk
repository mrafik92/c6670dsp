################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
obj/platforms_utils_test.obj: ../src/platforms_utils_test.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/C6000 Code Generation Tools 7.4.6/bin/cl6x" -mv6600 --abi=eabi -g --include_path="C:/ti/C6000 Code Generation Tools 7.4.6/include" --include_path="C:/ti/pdk_C6670_1_1_2_6/packages/ti/platform" --include_path="C:/" --include_path="C:/Users/csl" --include_path="C:/ti/pdk_C6670_1_1_2_6/packages/ti/csl" --include_path="C:/ti/pdk_C6670_1_1_2_6/packages/" --include_path="C:/Users/mahmoud" --display_error_number --diag_warning=225 --obj_directory="obj" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



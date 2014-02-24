################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/evmc6670_phy.obj: ../src/evmc6670_phy.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/C6000 Code Generation Tools 7.4.6/bin/cl6x" -mv6600 -g --include_path="C:/ti/C6000 Code Generation Tools 7.4.6/include" --include_path="C:/ti/pdk_C6670_1_1_2_6/packages/ti/platform" --include_path="C:/ti/pdk_C6670_1_1_2_6/packages/" --include_path="C:/ti/pdk_C6670_1_1_2_6/packages/ti/csl" --include_path="C:/Users/mahmoud/workspace_v5_5/platform_lib_simc6670/include" --include_path="C:/" --include_path="C:/Users/csl" --include_path="C:/Users/mahmoud" --diag_warning=225 --abi=eabi --preproc_with_compile --preproc_dependency="src/evmc6670_phy.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/platform_sim.obj: ../src/platform_sim.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/C6000 Code Generation Tools 7.4.6/bin/cl6x" -mv6600 -g --include_path="C:/ti/C6000 Code Generation Tools 7.4.6/include" --include_path="C:/ti/pdk_C6670_1_1_2_6/packages/ti/platform" --include_path="C:/ti/pdk_C6670_1_1_2_6/packages/" --include_path="C:/ti/pdk_C6670_1_1_2_6/packages/ti/csl" --include_path="C:/Users/mahmoud/workspace_v5_5/platform_lib_simc6670/include" --include_path="C:/" --include_path="C:/Users/csl" --include_path="C:/Users/mahmoud" --diag_warning=225 --abi=eabi --preproc_with_compile --preproc_dependency="src/platform_sim.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# 所有目标
all: openwrt-hhh

# 工具调用
openwrt-hhh: $(OBJS) $(USER_OBJS)
	@echo '正在构建目标： $@'
	@echo '正在调用： Cross G++ Linker'
	x86_64-openwrt-linux-musl-g++ -L/home/peter/transplant_srt/temp/lib -L/home/peter/lede/staging_dir/toolchain-x86_64_gcc-8.4.0_musl/lib -o "openwrt-hhh" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo '已结束构建目标： $@'
	@echo ' '

# 其他目标
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) openwrt-hhh
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets

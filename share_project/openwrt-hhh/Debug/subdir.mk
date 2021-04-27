################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HookRecv.c \
../OpenWrtClient.c \
../QueueRecv.c \
../RingBuffer.c \
../SrtSend.c 

OBJS += \
./HookRecv.o \
./OpenWrtClient.o \
./QueueRecv.o \
./RingBuffer.o \
./SrtSend.o 

C_DEPS += \
./HookRecv.d \
./OpenWrtClient.d \
./QueueRecv.d \
./RingBuffer.d \
./SrtSend.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Cross GCC Compiler'
	x86_64-openwrt-linux-musl-gcc -I/home/peter/transplant_srt/temp/include -I/home/peter/lede/staging_dir/target-x86_64_musl/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '



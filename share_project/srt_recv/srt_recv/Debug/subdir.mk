################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Main.c \
../RingBuffer.c \
../RtpSend.c \
../SrtRecv.c 

OBJS += \
./Main.o \
./RingBuffer.o \
./RtpSend.o \
./SrtRecv.o 

C_DEPS += \
./Main.d \
./RingBuffer.d \
./RtpSend.d \
./SrtRecv.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo '正在构建文件： $<'
	@echo '正在调用： GCC C Compiler'
	gcc -I/home/peter/transplant_srt/temp/include -I/usr/include -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '



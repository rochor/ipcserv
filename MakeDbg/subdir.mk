################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../client.c \
../linkedlist.c \
../linkedlist_mac.c \
../linkedlist_ip.c \
../linkedlist_pid.c \
../server.c \
../shmem.c \
../test.c 

CLIENT_OBJS += \
./client.o \
./linkedlist.o \
./linkedlist_mac.o \
./linkedlist_ip.o \
./linkedlist_pid.o \
./shmem.o \
./test.o 

SERVER_OBJS += \
./linkedlist.o \
./linkedlist_mac.o \
./linkedlist_ip.o \
./linkedlist_pid.o \
./server.o \
./shmem.o \
./test.o 

C_DEPS += \
./client.d \
./linkedlist.d \
./linkedlist_mac.d \
./linkedlist_ip.d \
./linkedlist_pid.d \
./server.d \
./shmem.d \
./test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



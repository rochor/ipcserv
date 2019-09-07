################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/client.c \
../src/linkedlist.c \
../src/linkedlist_mac.c \
../src/server.c \
../src/shmem.c \
../src/test.c 

CLIENT_OBJS += \
./client.o \
./linkedlist.o \
./linkedlist_mac.o \
./shmem.o \
./test.o 

SERVER_OBJS += \
./linkedlist.o \
./linkedlist_mac.o \
./server.o \
./shmem.o \
./test.o 

C_DEPS += \
./client.d \
./linkedlist.d \
./linkedlist_mac.d \
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



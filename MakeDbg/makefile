################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
all: ipcserv

# Tool invocations
ipcserv: $(SERVER_OBJS) $(CLIENT_OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "server" $(SERVER_OBJS) $(USER_OBJS) $(LIBS) -lrt #-lrt = link rt library (with shm_open)
	gcc  -o "client" $(CLIENT_OBJS) $(USER_OBJS) $(LIBS) -lrt
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(SERVER_OBJS)$(CLIENT_OBJS)$(C_DEPS) server client
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets

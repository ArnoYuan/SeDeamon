################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Source/Utils/ApplicationManager.cpp 

OBJS += \
./Source/Utils/ApplicationManager.o 

CPP_DEPS += \
./Source/Utils/ApplicationManager.d 


# Each subdirectory must supply rules for building sources it contributes
Source/Utils/%.o: ../Source/Utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-openwrt-linux-muslgnueabi-g++ -I$(SENAVICOMMON_PATH)/Source -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



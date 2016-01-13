################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AStar/AStar.cpp \
../src/AStar/AStarNode.cpp \
../src/AStar/micropather.cpp 

OBJS += \
./src/AStar/AStar.o \
./src/AStar/AStarNode.o \
./src/AStar/micropather.o 

CPP_DEPS += \
./src/AStar/AStar.d \
./src/AStar/AStarNode.d \
./src/AStar/micropather.d 


# Each subdirectory must supply rules for building sources it contributes
src/AStar/%.o: ../src/AStar/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/kaha/RU/workspaceC/GatewayHeuristic/src" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



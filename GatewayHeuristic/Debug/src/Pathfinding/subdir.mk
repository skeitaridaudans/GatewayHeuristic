################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Pathfinding/PathfindingStateSpace.cpp \
../src/Pathfinding/PathfindingStateSpaceMP.cpp \
../src/Pathfinding/Tile.cpp \
../src/Pathfinding/TraversibleMap.cpp 

OBJS += \
./src/Pathfinding/PathfindingStateSpace.o \
./src/Pathfinding/PathfindingStateSpaceMP.o \
./src/Pathfinding/Tile.o \
./src/Pathfinding/TraversibleMap.o 

CPP_DEPS += \
./src/Pathfinding/PathfindingStateSpace.d \
./src/Pathfinding/PathfindingStateSpaceMP.d \
./src/Pathfinding/Tile.d \
./src/Pathfinding/TraversibleMap.d 


# Each subdirectory must supply rules for building sources it contributes
src/Pathfinding/%.o: ../src/Pathfinding/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/kaha/RU/workspaceC/GatewayHeuristic/src" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



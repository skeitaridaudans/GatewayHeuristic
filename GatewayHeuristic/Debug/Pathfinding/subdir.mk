################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Pathfinding/PathfindingStateSpace.cpp \
../Pathfinding/Tile.cpp \
../Pathfinding/TraversibleMap.cpp 

OBJS += \
./Pathfinding/PathfindingStateSpace.o \
./Pathfinding/Tile.o \
./Pathfinding/TraversibleMap.o 

CPP_DEPS += \
./Pathfinding/PathfindingStateSpace.d \
./Pathfinding/Tile.d \
./Pathfinding/TraversibleMap.d 


# Each subdirectory must supply rules for building sources it contributes
Pathfinding/%.o: ../Pathfinding/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



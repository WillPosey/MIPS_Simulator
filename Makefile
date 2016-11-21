#################################################################################################
#												#
#	Author: William Posey									#
#	Course: CDA5155										#
#	Project 2: MIPS Simulator								#
#	Makefile										#
#												#
#################################################################################################
FLAGS	= -std=c++11 -g -Wall -Wno-unused-variable -Wno-unused-function
LINK	= 
SRC	= $(wildcard *.cpp)
OBJ	= $(SRC:.cpp=.o)
TARGET	= MIPSsim

all 		: $(TARGET)

$(TARGET)	: $(OBJ)
		g++ $(LINK) $(FLAGS) -o $@ $(OBJ)
			
.cpp.o		:
		g++ $(FLAGS) -c $< -o $@

.PHONY		: clean

clean 		: 
		rm *.o $(TARGET)

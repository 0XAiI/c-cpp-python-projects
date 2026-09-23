comma := ,
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lraylib -lm

SRC = main.cpp Game.cpp Space.cpp alien.cpp Laser.cpp
EXEC = main

ifeq ($(OS),Windows_NT)
	EXEC := $(EXEC).exe
	RM = del /f /q
else
	RM = rm -f
endif

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXEC) $(LDFLAGS) -Wl$(comma)--subsystem$(comma)console

clean:
	$(RM) $(EXEC)

run: all
	./$(EXEC)

.PHONY: all clean run

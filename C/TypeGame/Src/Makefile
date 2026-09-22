CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lraylib -lm

SRC = ../Include/Init.c ../Include/Type.c ../Include/User_Data.c main.c
EXEC = main

ifeq ($(OS),Windows_NT)
	EXEC := $(EXEC).exe
	RM = del /f /q
	LDFLAGS = -lraylib -lm -lopengl32 -lgdi32 -lwinmm
else
	RM = rm -f
endif

all:
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) $(LDFLAGS)

clean:
	$(RM) $(EXEC)

run: all
	./$(EXEC)

.PHONY: all clean run
CC = clang
CFLAGS = -Wall -Wextra
LDFLAGS = -lSDL2 -lSDL2_image -lm

SRC = main.c
EXEC = main

ifeq ($(OS),Windows_NT)
	EXEC := $(EXEC).exe
	RM = del /f /q
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lm
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
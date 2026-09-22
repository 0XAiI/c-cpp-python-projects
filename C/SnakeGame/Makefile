CC = clang
CFLAGS = -Wall -Wextra
LDFLAGS = -lraylib -lm

SRC = Snake.c
EXEC = Snake

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
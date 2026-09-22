CC = clang
CFLAGS = -Wall -Wextra
LDFLAGS = -lraylib -lm

SRC = Match3.c
EXEC = match3

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
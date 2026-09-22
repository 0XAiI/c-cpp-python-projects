CC = clang
CFLAGS = -Wall -Wextra
LDFLAGS = -lcs50 -lraylib -lm

SRC = Functions.c
EXEC = Functions

ifeq ($(OS),Windows_NT)
	EXEC := $(EXEC).exe
	RM = del /f /q
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
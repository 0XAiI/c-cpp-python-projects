CC = clang
CFLAGS = -Wall -Wextra -g

# Source files (qrcodegen.c is compiled directly, not linked separately)
SRC = main.c TinyPngOut.c qrcodegen.c
EXEC = main

ifeq ($(OS),Windows_NT)
    EXEC := $(EXEC).exe
    RM = del /f /q
    INCLUDES = -I cs50/include
    LDFLAGS = cs50.lib
else
    RM = rm -f
    INCLUDES = -I. -I/usr/local/include
    LDFLAGS = -L/usr/local/lib -lcs50
endif

all:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(EXEC) $(LDFLAGS)

clean:
	$(RM) $(EXEC)

run: all
	./$(EXEC)

.PHONY: all clean run

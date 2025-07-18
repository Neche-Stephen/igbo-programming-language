CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I./src
SRC = src/main.c src/token.c src/ast.c src/util.c src/lexer.c src/parser.c src/interpreter.c
OBJ = $(SRC:.c=.o)
TARGET = igbo

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

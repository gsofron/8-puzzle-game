# Makefile used to compile the game on Linux systems

EXEC := 8puzzle

LIB := ./lib
INCLUDE := ./include ./include/raylib
SRC := ./src

CC = gcc
CFLAGS = -Wall -Wextra -O2 $(addprefix -I,$(INCLUDE))
LDFLAGS = $(LIB)/libraylib_linux.a -lm -ldl -lpthread -lGL
export DISPLAY ?= :0 # mainly for WSL

OBJS := $(SRC)/PriorityQueue.o $(SRC)/puzzle.o $(SRC)/interface.o $(SRC)/main.o

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: clean
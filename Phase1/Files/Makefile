# ***********************************************
#
# @File: Makefile
# @Purpose: Build system for the Citadel System
#           Phase 1 - Maester process.
# @Author: Anas TAGUI |  Abderrahmen MESKINE
# @Date: 2026-03-06
#
# ***********************************************

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -g
TARGET = Maester

# Source files
SRCS = main.c io_utils.c config.c inventory.c commands.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
main.o: main.c main.h globals.h io_utils.h config.h inventory.h commands.h
	$(CC) $(CFLAGS) -c main.c

io_utils.o: io_utils.c io_utils.h globals.h
	$(CC) $(CFLAGS) -c io_utils.c

config.o: config.c config.h globals.h io_utils.h
	$(CC) $(CFLAGS) -c config.c

inventory.o: inventory.c inventory.h globals.h io_utils.h
	$(CC) $(CFLAGS) -c inventory.c

commands.o: commands.c commands.h globals.h io_utils.h config.h inventory.h
	$(CC) $(CFLAGS) -c commands.c

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean

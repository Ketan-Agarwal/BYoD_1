# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Executable name
TARGET = main

# Source and object files
SRCS = main.c functions.c myutils.c tree.c cache.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f *.o $(TARGET)

# Optional: Add a phony target to avoid conflicts with files named 'clean' or 'all'
.PHONY: all clean
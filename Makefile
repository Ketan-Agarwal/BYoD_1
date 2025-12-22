CC = gcc
CFLAGS = -Wall -Wextra -g

MAIN_SOURCES = main.c functions.c myutils.c cache.c
BTREE_SOURCES = Btree/btree.c Btree/node.c Btree/butils.c

SOURCES = $(MAIN_SOURCES) $(BTREE_SOURCES)
OBJECTS = $(SOURCES:.c=.o)
TARGET = main

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f Btree/*.o

.PHONY: clean

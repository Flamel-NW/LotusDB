TARGET := main

SOURCES := $(wildcard *.c) $(wildcard */*.c)

OBJECTS := $(SOURCES:.c=.o)

INCLUDE := -I ./ \
	-I ./pch \
	-I ./skiplist \
	-I ./memtable \
	-I ./wal 

CFLAGS := -g -Wall -Werror -Wno-unused

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

SOURCES := main.c \
	./skiplist/skiplist.c

OBJECTS := $(SOURCES:.c=.o)

INCLUDE := -I ./skiplist

LotusDB: $(OBJECTS)
	$(CC) $(INCLUDE) $^ -o $@

clean:
	rm -f LotusDB $(OBJECTS) compile_commands.json

%.o: %.c
	$(CC) $(INCLUDE) -o $@ -c $<

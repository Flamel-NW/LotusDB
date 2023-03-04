SOURCES := main.c \
	LotusDB.c \
	./pch/pch.c \
	./skiplist/skiplist.c \
	./memtable/memtable.c \
	./wal/wal_entry.c

OBJECTS := $(SOURCES:.c=.o)

INCLUDE := -I ./ \
	-I ./pch \
	-I ./skiplist \
	-I ./memtable \
	-I ./wal 

LotusDB: $(OBJECTS)
	$(CC) -g $(INCLUDE) $^ -o $@

clean:
	rm -f LotusDB $(OBJECTS) compile_commands.json

%.o: %.c
	$(CC) -g $(INCLUDE) -o $@ -c $<

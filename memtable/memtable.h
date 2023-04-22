#ifndef MEMTABLE_H
#define MEMTABLE_H

#include "skiplist.h"
#include "wal_entry.h"

typedef struct memtable {
    SkipList* skip_list;
    pthread_rwlock_t rwlock;
    FILE* fp;
    size_t size;
} Memtable;

Memtable* initMemtable();

void addMemtable(Memtable* memtable, WalEntry* wal_entry);

static inline WalEntry* getMemtable(Memtable* memtable, const char* key) {
    return getSkipList(memtable->skip_list, key);
}

void delMemtable(Memtable* memtable);

#endif

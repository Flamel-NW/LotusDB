#ifndef MEMTABLE_H
#define MEMTABLE_H

#include "skiplist.h"
#include <stdint.h>

typedef struct memtable {
    SkipList* skip_list;
    pthread_rwlock_t rwlock;
    FILE* fp;
} Memtable;

Memtable* initMemtable();

void delMemtable(Memtable* memtable);

#endif

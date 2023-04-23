#ifndef MEMTABLE_H
#define MEMTABLE_H

#include "skiplist.h"
#include "wal_entry.h"
#include "index.h"

typedef struct memtable {
    SkipList* skip_list;
    pthread_rwlock_t rwlock;
    FILE* fp;
    size_t size;
} Memtable;

Memtable* initMemtable();

// 添加 & 修改 满了返回false
bool addMemtable(Memtable* memtable, const char* key, const char* value);

WalEntry* getMemtable(Memtable* memtable, const char* key);

void removeMemtable(Memtable* memtable, const char* key);

void delMemtable(Memtable* memtable);

Memtable* makeImmutable(Memtable* memtable, BTree* b_tree);

#endif

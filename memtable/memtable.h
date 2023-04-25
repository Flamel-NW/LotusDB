#ifndef MEMTABLE_H
#define MEMTABLE_H

#include "pch.h"

#include "skiplist.h"
#include "wal_entry.h"
#include "index.h"

typedef struct memtable {
    SkipList* skip_list;
    pthread_rwlock_t rwlock;
    FILE* fp;
    uint32_t size;
} Memtable;

Memtable* initMemtable();

// 添加 & 修改 满了返回false
bool addMemtable(Memtable* memtable, const char* key, const char* value);

WalEntry* getMemtable(Memtable* memtable, const char* key);

bool removeMemtable(Memtable* memtable, const char* key);

void delMemtable(Memtable* memtable);

Memtable* makeImmutable(Memtable* memtable, BTree* b_tree);

#endif

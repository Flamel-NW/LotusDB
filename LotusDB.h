#ifndef LOTUSDB_H
#define LOTUSDB_H

#include "pch.h"
#include "memtable.h"
#include "index.h"

typedef struct lotus_db {
    Memtable* Memtable;
} LotusDB;

extern BTree* g_index;

LotusDB* initLotusDb();

// 添加 & 修改
static inline void addLotusDB(LotusDB *db, const char* key, const char* value) {
    WalEntry* wal_entry = initWalEntry(key, value);
    addSkipList(db->Memtable->skip_list, wal_entry);
}

static inline char* getLotusDB(LotusDB *db, const char* key) {
    return getMemtable(db->Memtable, key)->value;
}

static inline void delLotusDb(LotusDB* db) {
    delMemtable(db->Memtable);
    free(db);
}


#endif

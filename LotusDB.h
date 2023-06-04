#ifndef LOTUSDB_H
#define LOTUSDB_H

#include "pch.h"

#include "memtable.h"
#include "index.h"
#include "vl_entry.h"

typedef struct lotus_db {
    Memtable* memtable;
    BTree* index;
} LotusDB;

LotusDB* initLotusDb();

void addLotusDB(LotusDB* db, const char* key, const char* value);
bool getLotusDB(LotusDB* db, const char* key, char* value);
void removeLotusDB(LotusDB* db, const char* key);
void delLotusDb(LotusDB* db);
void mergeLotusDb(LotusDB* db);

void testMergeLotusDb(LotusDB* db);

#endif

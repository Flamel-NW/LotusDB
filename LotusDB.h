#ifndef LOTUSDB_H
#define LOTUSDB_H

#include "memtable.h"
#include "pch.h"

typedef struct lotus_db {
    Memtable* Memtable;
} LotusDB;

LotusDB* initLotusDb();

void addLotusDB(LotusDB *db, const char* key, const char* value);

char* getLotusDB(LotusDB *db, const char* key);

#endif

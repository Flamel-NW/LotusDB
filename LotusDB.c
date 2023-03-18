#include "LotusDB.h"
#include "memtable.h"
#include "pch.h"
#include "skiplist.h"
#include "wal_entry.h"

#include <dirent.h>
#include <stdlib.h>

LotusDB* initLotusDb() {
    LotusDB* db = (LotusDB*) malloc(sizeof(LotusDB));

    DIR* dir;
    struct dirent* dirent;

    dir = opendir(WAL_NAME);

    db->Memtable = initMemtable();

    return db;
}

void addLotusDB(LotusDB *db, const char* key, const char* value) {
    WalEntry* wal_entry = initWalEntry(key, value);
    addWalEntryToSkipList(db->Memtable->skip_list, wal_entry);
}

char* getLotusDB(LotusDB *db, const char* key) {
    return getWalEntryFromSkipList(db->Memtable->skip_list, key)->value;
}

void delLotusDb(LotusDB* db) {
    delMemtable(db->Memtable);
    free(db);
}

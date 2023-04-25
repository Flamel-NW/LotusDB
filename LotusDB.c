#include "LotusDB.h"


LotusDB* initLotusDb() {
    LotusDB* db = (LotusDB*) malloc(sizeof(LotusDB));
    db->memtable = initMemtable();
    db->index = initBTree();
    return db;
}

// 添加 & 修改
void addLotusDB(LotusDB* db, const char* key, const char* value) {
    assert(key && strlen(key));
    if (!addMemtable(db->memtable, key, value)) {
        db->memtable = makeImmutable(db->memtable, db->index);
        addMemtable(db->memtable, key, value);
    }
}

bool getLotusDB(LotusDB* db, const char* key, char* value) {
    assert(key && strlen(key));
    WalEntry* wal_entry = getMemtable(db->memtable, key);
    if (!wal_entry) {
        Metadata* metadata = malloc(sizeof(Metadata) + strlen(key) + 1);
        if (!getBTree(db->index, key, metadata)) {
            free(metadata);
            return false;
        } else {
            VlEntry* vl_entry = getVlEntry(metadata);
            memcpy(value, vl_entry->value, vl_entry->value_size);
            assert(metadata->timestamp == vl_entry->timestamp);
            free(vl_entry);
            free(metadata);
            return true;
        }
    } 
    if (getWalEntryValue(wal_entry)) {
        memcpy(value, getWalEntryValue(wal_entry), wal_entry->value_size);
        free(wal_entry);
        return true;
    }
    free(wal_entry);
    return false;
}

void removeLotusDB(LotusDB* db, const char* key) {
    assert(key && strlen(key));
    if (!removeMemtable(db->memtable, key)) {
        db->memtable = makeImmutable(db->memtable, db->index);
        removeMemtable(db->memtable, key);
    }
}

void delLotusDb(LotusDB* db) {
    delMemtable(db->memtable);
    delBTree(db->index);
    saveVlFile();
    free(db);
}

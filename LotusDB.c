#include "LotusDB.h"

LotusDB* initLotusDb() {
    LotusDB* db = (LotusDB*) malloc(sizeof(LotusDB));
    db->memtable = initMemtable();
    db->index = initBTree();
    return db;
}

// 添加 & 修改
void addLotusDB(LotusDB* db, const char* key, const char* value) {
    if (!addMemtable(db->memtable, key, value)) {
        db->memtable = makeImmutable(db->memtable, db->index);
        addMemtable(db->memtable, key, value);
    }
}

char* getLotusDB(LotusDB* db, const char* key) {
    char* ret = getMemtable(db->memtable, key)->value;
    if (!ret) {
        // TODO:
    }
    return ret;
}

void removeLotusDB(LotusDB* db, const char* key) {
    removeMemtable(db->memtable, key);
}

void delLotusDb(LotusDB* db) {
    delMemtable(db->memtable);
    free(db);
}

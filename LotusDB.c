#include "LotusDB.h"

#include <dirent.h>

BTree* g_index;

LotusDB* initLotusDb() {
    LotusDB* db = (LotusDB*) malloc(sizeof(LotusDB));
    DIR* dir;
    struct dirent* dirent;
    dir = opendir(WAL_NAME);
    db->Memtable = initMemtable();
    g_index = initBTree();
    return db;
}

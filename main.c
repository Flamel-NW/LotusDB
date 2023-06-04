#include "LotusDB.h"


int main() {
    LotusDB* db;

    char test_key[KEY_MAX_LEN];
    char test_value[VALUE_MAX_LEN];
    char temp_value[VALUE_MAX_LEN];

    // 写入16k个KV对
    db = initLotusDb();
    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        sprintf(test_key, "key: %d", i);
        sprintf(test_value, "value: %d", i);
        addLotusDB(db, test_key, test_value);
        // STDERR("add key: \"%s\"", test_key);
        // STDERR("add value: \"%s\"\n", test_value);
    }

    delLotusDb(db);
    db = initLotusDb();

    // 查询所有KV对
    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        sprintf(test_key, "key: %d", i);
        memset(test_value, 0, sizeof(test_value));
        getLotusDB(db, test_key, test_value);

        // STDERR("get key: \"%s\"", test_key);
        // STDERR("get value: \"%s\"\n", test_value);

        sprintf(temp_value, "value: %d", i);
        assert(strcmp(temp_value, test_value) == 0);
    }

    delLotusDb(db);
    db = initLotusDb();

    // 删除奇数编号的KV对
    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        if (i % 2) {
            sprintf(test_key, "key: %d", i);
            removeLotusDB(db, test_key);

            // STDERR("remove key: \"%s\"", test_key);

            assert(getLotusDB(db, test_key, test_value) == false);
        }
    }

    delLotusDb(db);
    db = initLotusDb();

    // 查询所有KV对
    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        sprintf(test_key, "key: %d", i);
        if (getLotusDB(db, test_key, test_value)) {
            // STDERR("get key: \"%s\"", test_key);
            // STDERR("get value: \"%s\"\n", test_value);

            sprintf(temp_value, "value: %d", i);
            assert(strcmp(temp_value, test_value) == 0);
        } else {
            // STDERR("not found");
        }
    }

    // STDERR("GC begin");
    // uint64_t timestamp = getTimestamp();
    // mergeLotusDb(db);
    // STDERR("GC end");
    // STDERR("GC time: %ldms", (getTimestamp() - timestamp) / 1000);

    // STDERR("Fake GC begin");
    // uint64_t timestamp = getTimestamp();
    // testMergeLotusDb(db);
    // STDERR("Fake GC end");
    // STDERR("Fake GC time: %ldms", (getTimestamp() - timestamp) / 1000);

    delLotusDb(db);
    return 0;
}

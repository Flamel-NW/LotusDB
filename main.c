#include "LotusDB.h"


int main() {
    LotusDB* db;

    char test_key[KEY_MAX_LEN];
    char test_value[VALUE_MAX_LEN];
    char temp_value[VALUE_MAX_LEN];

    db = initLotusDb();
    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        sprintf(test_key, "key: %d", i);
        sprintf(test_value, "value: %d", i);
        addLotusDB(db, test_key, test_value);
        STDERR("add key: \"%s\"", test_key);
        STDERR("add value: \"%s\"\n", test_value);
    }

    delLotusDb(db);
    db = initLotusDb();

    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        sprintf(test_key, "key: %d", i);
        getLotusDB(db, test_key, test_value);

        STDERR("get key: \"%s\"", test_key);
        STDERR("get value: \"%s\"\n", test_value);

        sprintf(temp_value, "value: %d", i);
        assert(strcmp(temp_value, test_value) == 0);
    }

    delLotusDb(db);
    db = initLotusDb();

    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        if (i % 2) {
            if (i == 379)
                STDERR_FUNC_LINE();

            sprintf(test_key, "key: %d", i);
            removeLotusDB(db, test_key);

            STDERR("remove key: \"%s\"", test_key);

            assert(getLotusDB(db, test_key, test_value) == false);
        }
    }
    
    delLotusDb(db);
    db = initLotusDb();

    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        if (!(i % 2)) {
            sprintf(test_key, "key: %d", i);
            getLotusDB(db, test_key, test_value);

            STDERR("get key: \"%s\"", test_key);
            STDERR("get value: \"%s\"\n", test_value);

            sprintf(temp_value, "value: %d", i);
            assert(strcmp(temp_value, test_value) == 0);
        }
    }

    delLotusDb(db);
    return 0;
}

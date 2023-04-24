#include "LotusDB.h"


int main() {
    LotusDB* db = initLotusDb();

    char test_key[KEY_MAX_LEN];
    char test_value[VALUE_MAX_LEN];

    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        sprintf(test_key, "key: %d", i);
        sprintf(test_value, "value: %d", i);
        if (i == 799)
            STDERR_FUNC_LINE();
        addLotusDB(db, test_key, test_value);
        STDERR("add key: \"%s\"", test_key);
        STDERR("add value: \"%s\"\n", test_value);
    }

    char temp_value[VALUE_MAX_LEN];
    for (int32_t i = 0; i < PAGE_SIZE; i++) {
        sprintf(test_key, "key: %d", i);
        getLotusDB(db, test_key, test_value);

        STDERR("get key: \"%s\"", test_key);
        STDERR("get value: \"%s\"\n", test_value);

        sprintf(temp_value, "value: %d", i);
        assert(strcmp(temp_value, test_value) == 0);
    }

    delLotusDb(db);
    return 0;
}

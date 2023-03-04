#include "LotusDB.h"
#include <stdio.h>

int main() {
    LotusDB* db = initLotusDb();

    addLotusDB(db, "a", "1");
    addLotusDB(db, "b", "2");
    addLotusDB(db, "c", "3");

    printf("%s\n", getLotusDB(db, "c"));

    return 0;
}

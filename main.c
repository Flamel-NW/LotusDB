#include "LotusDB.h"
#include <stdio.h>

int main() {
    LotusDB* db = initLotusDb();


    printf("%s\n", getLotusDB(db, "b"));

    delLotusDb(db);
    return 0;
}

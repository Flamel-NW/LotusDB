#include "LotusDB.h"


int main() {
    LotusDB* db = initLotusDb();

    // addLotusDB(db, "a", "1");
    // addLotusDB(db, "b", "2");
    // addLotusDB(db, "t", "3");

    printf("%s\n", getLotusDB(db, "b"));

    delLotusDb(db);
    return 0;
}

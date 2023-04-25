#include "wal_entry.h"


// The WalEntry looks like:
// +-------+-----------+----------+------------+-------+---------+
// |  crc  | timestamp | key_size | value_size |  key  |  value  |
// +-------+-----------+----------+------------+-------+---------+
// |---4---|-----8-----|----4-----|-----4------|
// |------------------header-------------------|
//         |----------------------crc check----------------------|

WalEntry* initWalEntry(const char* key, const char* value) {
    if (strlen(key) + 1 > KEY_MAX_LEN || sizeof(value) + 1 > VALUE_MAX_LEN)
        return NULL;

    WalEntry* wal_entry = NULL;
    if (value) {
        wal_entry = malloc(sizeof(WalEntry) + strlen(key) + strlen(value) + 2);
        wal_entry->timestamp = getTimestamp();
        wal_entry->key_size = strlen(key) + 1;
        wal_entry->value_size = strlen(value) + 1;
        memcpy(getWalEntryKey(wal_entry), key, wal_entry->key_size);
        memcpy(getWalEntryValue(wal_entry), value, wal_entry->value_size);
    } else {
        wal_entry = malloc(sizeof(WalEntry) + strlen(key) + 1);
        wal_entry->timestamp = getTimestamp();
        wal_entry->key_size = strlen(key) + 1;
        wal_entry->value_size = 0;
        memcpy(getWalEntryKey(wal_entry), key, wal_entry->key_size);
    }

    wal_entry->crc = getWalEntryCrc(wal_entry);
    return wal_entry;
}

WalEntry* loadWalEntry(FILE *wal) {
    WalEntry wal_header;
    fread(&wal_header, sizeof(WalEntry), 1, wal);
    
    WalEntry* wal_entry = malloc(sizeof(WalEntry) + 
        wal_header.key_size + wal_header.value_size);
    *wal_entry = wal_header;
    fread(getWalEntryKey(wal_entry), wal_entry->key_size, 1, wal);
    if (getWalEntryValue(wal_entry))
        fread(getWalEntryValue(wal_entry), wal_entry->value_size, 1, wal);

    if (feof(wal)) {
        free(wal_entry);
        return NULL;
    } else {
        assert(getWalEntryCrc(wal_entry) == wal_entry->crc);
        return wal_entry;
    }
}

void saveWalEntry(WalEntry* wal_entry, FILE *wal) {
    fwrite(wal_entry, getWalEntrySize(wal_entry), 1, wal);
}

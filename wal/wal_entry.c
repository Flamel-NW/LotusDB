#include "wal_entry.h"


// The WalEntry looks like:
// +-------+-----------+----------+------------+-------+---------+
// |  crc  | timestamp | key_size | value_size |  key  |  value  |
// +-------+-----------+----------+------------+-------+---------+
// |---4---|-----8-----|----8-----|-----8------|
// |------------------header-------------------|
//         |----------------------crc check----------------------|

static uint32_t getWalEntryCrc(WalEntry* wal_entry) {
    static byte buffer[sizeof(WalHeader) + KEY_MAX_LEN + VALUE_MAX_LEN];
    size_t index = 0;

    memcpy(&buffer[index], &wal_entry->header.timestamp, sizeof(wal_entry->header.timestamp));
    index += sizeof(wal_entry->header.timestamp);

    memcpy(&buffer[index], &wal_entry->header.key_size, sizeof(wal_entry->header.key_size));
    index += sizeof(wal_entry->header.key_size);

    if (wal_entry->header.value_size) {
        memcpy(&buffer[index], &wal_entry->header.value_size, sizeof(wal_entry->header.value_size));
        index += sizeof(wal_entry->header.value_size);
    }

    memcpy(&buffer[index], wal_entry->key, strlen(wal_entry->key) + 1);
    index += strlen(wal_entry->key) + 1;

    if (wal_entry->value) {
        memcpy(&buffer[index], wal_entry->value, strlen(wal_entry->value) + 1);
        index += strlen(wal_entry->value) + 1;
    }

    return crc32(buffer, index);
}

WalEntry* initWalEntry(const char* key, const char* value) {
    if (strlen(key) + 1 > KEY_MAX_LEN || sizeof(value) + 1 > VALUE_MAX_LEN)
        return NULL;

    WalEntry* wal_entry = (WalEntry*) malloc(sizeof(WalEntry));

    wal_entry->header.timestamp = getTimestamp();
    wal_entry->header.key_size = strlen(key) + 1;
    wal_entry->header.value_size = value ? strlen(value) + 1 : 0;

    wal_entry->key = strdup(key);
    wal_entry->value = value ? strdup(value) : NULL;

    wal_entry->header.crc = getWalEntryCrc(wal_entry);

    return wal_entry;
}

WalEntry* loadWalEntry(FILE *wal) {
    WalEntry* wal_entry = (WalEntry*) malloc(sizeof(WalEntry));

    fread(&wal_entry->header, sizeof(WalHeader), 1, wal);

    wal_entry->key = (char*) malloc(wal_entry->header.key_size);
    wal_entry->value = wal_entry->header.value_size ? 
        (char*) malloc(wal_entry->header.value_size) : NULL;

    fread(wal_entry->key, sizeof(char), wal_entry->header.key_size, wal);

    if (wal_entry->value)
        fread(wal_entry->value, sizeof(char), wal_entry->header.value_size, wal);

    if (feof(wal)) {
        free(wal_entry);
        return NULL;
    } else {
        assert(getWalEntryCrc(wal_entry) == wal_entry->header.crc);
        return wal_entry;
    }
}

void saveWalEntry(WalEntry* wal_entry, FILE *wal) {
    fwrite(&(wal_entry->header), sizeof(wal_entry->header), 1, wal);
    fwrite(wal_entry->key, sizeof(char), wal_entry->header.key_size, wal);
    if (wal_entry->header.value_size && wal_entry->value)
        fwrite(wal_entry->value, sizeof(char), wal_entry->header.value_size, wal);
}

void delWalEntry(WalEntry* wal_entry) {
    free(wal_entry->key);
    if (wal_entry->header.value_size && wal_entry->value)
        free(wal_entry->value);
    free(wal_entry);
}

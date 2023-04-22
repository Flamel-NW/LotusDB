#ifndef WAL_H
#define WAL_H

#include "pch.h"

// The WalEntry looks like:
// +-------+-----------+----------+------------+-------+---------+
// |  crc  | timestamp | key_size | value_size |  key  |  value  |
// +-------+-----------+----------+------------+-------+---------+
// |---4---|-----4-----|----8-----|-----8------|
// |------------------header-------------------|
//         |----------------------crc check----------------------|

#pragma pack(4)
typedef struct wal_header {
    uint32_t crc;
    int32_t timestamp;
    size_t key_size;
    size_t value_size;
} WalHeader;

typedef struct wal_entry {
    WalHeader header;
    char* key;
    char* value;
} WalEntry;
#pragma pack()

static inline size_t getWalEntrySize(WalEntry* wal_entry) {
    return sizeof(wal_entry->header) +
        wal_entry->header.key_size + wal_entry->header.value_size;
}

WalEntry* initWalEntry(const char* key, const char* value);

WalEntry* loadWalEntry(FILE* wal);


static inline void saveWalEntry(WalEntry* wal_entry, FILE *wal) {
    fwrite(&(wal_entry->header), sizeof(wal_entry->header), 1, wal);
    fwrite(wal_entry->key, sizeof(char), wal_entry->header.key_size, wal);
    fwrite(wal_entry->value, sizeof(char), wal_entry->header.value_size, wal);
}

static inline void delWalEntry(WalEntry* wal_entry) {
    free(wal_entry->key);
    free(wal_entry->value);
    free(wal_entry);
}

#endif

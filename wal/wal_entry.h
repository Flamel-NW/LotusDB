#ifndef WAL_H
#define WAL_H

#include "pch.h"

// The WalEntry looks like:
// +-------+-----------+----------+------------+-------+---------+
// |  crc  | timestamp | key_size | value_size |  key  |  value  |
// +-------+-----------+----------+------------+-------+---------+
// |---4---|-----8-----|----4-----|-----4------|
// |------------------header-------------------|
//         |----------------------crc check----------------------|

// value_size == 0 代表删除这个key

#pragma pack(4)
typedef struct wal_entry {
    uint32_t crc;
    uint64_t timestamp;
    uint32_t key_size;
    uint32_t value_size;
    char key_value[];
} WalEntry;
#pragma pack()

static inline uint32_t getWalEntrySize(WalEntry* wal_entry) {
    return sizeof(WalEntry) + wal_entry->key_size + wal_entry->value_size;
}

static inline char* getWalEntryKey(WalEntry* wal_entry) {
    return wal_entry->key_value;
}

static inline char* getWalEntryValue(WalEntry* wal_entry) {
    if (wal_entry->value_size)
        return &wal_entry->key_value[wal_entry->key_size];
    else
        return NULL;
}

static inline uint32_t getWalEntryCrc(WalEntry* wal_entry) {
    return crc32((const byte*) wal_entry + CRC_SHIFT, 
        getWalEntrySize(wal_entry) - CRC_SHIFT);
}

WalEntry* initWalEntry(const char* key, const char* value);

WalEntry* loadWalEntry(FILE* wal);

void saveWalEntry(WalEntry* wal_entry, FILE *wal);

#endif

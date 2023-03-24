#ifndef WAL_H
#define WAL_H

#include "pch.h"

// The WalEntry looks like:
// +-------+-----------+----------+------------+-------+---------+
// |  crc  | timestamp | key_size | value_size |  key  |  value  |
// +-------+-----------+----------+------------+-------+---------+
// |---4---|-----4-----|----4-----|-----4------|
// |------------------header-------------------|
//         |----------------------crc check----------------------|

typedef struct wal_header {
    int32_t crc;
    int32_t timestamp;
    size_t key_size;
    size_t value_size;
} WalHeader;

typedef struct wal_entry {
    WalHeader header;
    char* key;
    char* value;
} WalEntry;

WalEntry* initWalEntry(const char* key, const char* value);

WalEntry* readWalEntryFromWal(FILE* wal);

void writeWalEntryToWal(WalEntry* wal_entry, FILE* wal);

void delWalEntry(WalEntry* wal_entry);

#endif

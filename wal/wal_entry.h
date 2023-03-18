#ifndef WAL_H
#define WAL_H

#include "pch.h"

typedef struct wal_header {
    int32_t crc;
    int32_t timestamp;
    uint32_t key_size;
    uint32_t value_size;
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

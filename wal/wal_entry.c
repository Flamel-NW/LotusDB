#include "wal_entry.h"

#include "pch.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The WalEntry looks like:
// +-------+-----------+----------+------------+-------+---------+
// |  crc  | timestamp | key_size | value_size |  key  |  value  |
// +-------+-----------+----------+------------+-------+---------+
// |---4---|-----4-----|----4-----|-----4------|
// |------------------header-------------------|
//         |----------------------crc check----------------------|

WalEntry* initWalEntry(const char* key, const char* value) {
    if (strlen(key) > KEY_MAX_LEN || strlen(value) > VALUE_MAX_LEN)
        return NULL;

    WalEntry* wal_entry = (WalEntry*) malloc(sizeof(WalEntry));

    wal_entry->header.crc = 0;          // TODO: crc
    wal_entry->header.timestamp = getTimestamp();
    wal_entry->header.key_size = strlen(key);
    wal_entry->header.value_size = strlen(value);

    wal_entry->key = strdup(key);
    wal_entry->value = strdup(value);

    return wal_entry;
}

WalEntry* readWalEntryFromWal(FILE *wal) {
    WalEntry* wal_entry = (WalEntry*) malloc(sizeof(WalEntry));

    fread(&wal_entry->header, sizeof(WalHeader), 1, wal);

    wal_entry->key = (char*) malloc(wal_entry->header.key_size);
    wal_entry->value = (char*) malloc(wal_entry->header.value_size);

    fread(wal_entry->key, sizeof(char), wal_entry->header.key_size, wal);
    fread(wal_entry->value, sizeof(char), wal_entry->header.value_size, wal);

    if (feof(wal)) {
        free(wal_entry);
        return NULL;
    } else {
        return wal_entry;
    }
}

void writeWalEntryToWal(WalEntry wal_entry, FILE *wal) {
    fwrite(&(wal_entry.header), sizeof(wal_entry.header), 1, wal);
    fwrite(wal_entry.key, sizeof(char), wal_entry.header.key_size, wal);
    fwrite(wal_entry.value, sizeof(char), wal_entry.header.value_size, wal);
}


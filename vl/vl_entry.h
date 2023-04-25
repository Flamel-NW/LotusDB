#ifndef VL_ENTRY_H
#define VL_ENTRY_H

#include "pch.h"

#include "wal_entry.h"
#include "index.h"

// The VlEntry looks like:
// +-------+-----------+------------+---------+
// |  crc  | timestamp | value_size |  value  |
// +-------+-----------+------------+---------+
// |-------------HEADER-------------|
//         |------------crc check-------------|
// |---4---|-----8-----|-----4------|
// |---------------20---------------|

#pragma pack(4)
typedef struct vl_entry {
    uint32_t crc;
    uint64_t timestamp;
    uint32_t value_size;
    char value[];
} VlEntry;
#pragma pack()

static inline uint32_t getVlEntrySize(VlEntry* vl_entry) {
    return sizeof(VlEntry) + vl_entry->value_size;
}

static inline uint32_t getVlEntryCrc(VlEntry* vl_entry) {
    return crc32((const byte*) vl_entry + CRC_SHIFT, 
        getVlEntrySize(vl_entry) - CRC_SHIFT);
}

Metadata* addVlEntry(WalEntry* wal_entry);

VlEntry* getVlEntry(Metadata* metadata);

void saveVlFile();

#endif

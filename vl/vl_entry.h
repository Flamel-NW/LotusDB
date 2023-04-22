#ifndef VL_ENTRY_H
#define VL_ENTRY_H

#include "pch.h"

// The VlEntry looks like:
// +-------+-----------+------------+---------+
// |  crc  | timestamp | value_size |  value  |
// +-------+-----------+------------+---------+
// |-------------HEADER-------------|
//         |------------crc check-------------|
// |---4---|-----4-----|-----8------|
// |---------------16---------------|

#pragma pack(4)
typedef struct vl_entry {
    uint32_t crc;
    int32_t timestamp;
    size_t value_size;
    char value[];
} VlEntry;
#pragma pack()

#endif

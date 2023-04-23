#include "vl_entry.h"

#include "index.h"
#include "wal_entry.h"
#include "LotusDB.h"

#define VL_FILE_MAX_SIZE PAGE_SIZE


static int32_t g_curr_vl_id = -1;
static uint32_t g_curr_vl_offset = -1;
static byte g_curr_vl_data[VL_FILE_MAX_SIZE];
    
static inline uint32_t getVlEntryCrc(VlEntry* vl_entry) {
    return crc32((const byte*) vl_entry, sizeof(VlEntry) + vl_entry->value_size);
}

Metadata* addVlEntry(WalEntry* wal_entry) {
    if (g_curr_vl_id == -1 && g_curr_vl_offset == -1) {
        g_curr_vl_id = getTimestamp();
        g_curr_vl_offset = 0;
    }

    size_t vl_size = sizeof(VlEntry) + strlen(wal_entry->value) + 1;
    if (vl_size + g_curr_vl_offset > VL_FILE_MAX_SIZE) {
        static char buffer[32];
        sprintf(buffer, "%s%x", VL_PATH, g_curr_vl_id);
        FILE* vl_file = fopen(buffer, "wb");
        fwrite(g_curr_vl_data, sizeof(byte), VL_FILE_MAX_SIZE, vl_file);
        fclose(vl_file);

        g_curr_vl_id = getTimestamp();
        g_curr_vl_offset = 0;
    }

    VlEntry* vl_entry = (VlEntry*) &g_curr_vl_data[g_curr_vl_offset];
    vl_entry->timestamp = wal_entry->header.timestamp;
    vl_entry->value_size = wal_entry->header.value_size;
    memcpy(vl_entry->value, wal_entry->value, vl_entry->value_size);

    Metadata* metadata = malloc(sizeof(Metadata) + wal_entry->header.key_size);
    metadata->vl_id = g_curr_vl_id;
    metadata->offset = g_curr_vl_offset;
    metadata->timestamp = wal_entry->header.timestamp;
    metadata->key_size = wal_entry->header.key_size;
    metadata->crc = getMetadataCrc(metadata);

    g_curr_vl_offset += vl_size;

    return metadata;
}

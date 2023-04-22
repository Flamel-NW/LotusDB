#include "vl_entry.h"

#include "index.h"
#include "wal_entry.h"
#include "LotusDB.h"

#define VL_FILE_MAX_SIZE PAGE_SIZE

static inline uint32_t getVlEntryCrc(VlEntry* vl_entry) {
    return crc32((const byte*) vl_entry, sizeof(VlEntry) + vl_entry->value_size);
}

void addVlEntry(WalEntry* wal_entry) {
    static int32_t curr_vl_id = -1;
    static uint32_t curr_vl_offset = -1;
    static byte curr_vl_data[VL_FILE_MAX_SIZE];
    if (curr_vl_id == -1 && curr_vl_offset == -1) {
        curr_vl_id = getTimestamp();
        curr_vl_offset = 0;
    }

    size_t vl_size = sizeof(VlEntry) + strlen(wal_entry->value) + 1;
    if (vl_size + curr_vl_offset > VL_FILE_MAX_SIZE) {
        static char buffer[32];
        sprintf(buffer, "%s%x", VL_PATH, curr_vl_id);
        FILE* vl_file = fopen(buffer, "wb");
        fwrite(curr_vl_data, sizeof(byte), VL_FILE_MAX_SIZE, vl_file);
        fclose(vl_file);

        curr_vl_id = getTimestamp();
        curr_vl_offset = 0;
    }

    VlEntry* vl_entry = (VlEntry*) &curr_vl_data[curr_vl_offset];
    vl_entry->timestamp = wal_entry->header.timestamp;
    vl_entry->value_size = wal_entry->header.value_size;
    memcpy(vl_entry->value, wal_entry->value, vl_entry->value_size);

    Metadata* metadata = malloc(sizeof(Metadata) + wal_entry->header.key_size);
    metadata->vl_id = curr_vl_id;
    metadata->offset = curr_vl_offset;
    metadata->timestamp = wal_entry->header.timestamp;
    metadata->key_size = wal_entry->header.key_size;
    metadata->crc = getMetadataCrc(metadata);
    addBTree(g_index, metadata);
    free(metadata);

    curr_vl_offset += vl_size;
}

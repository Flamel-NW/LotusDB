#include "vl_entry.h"

#define VL_FILE_MAX_SIZE PAGE_SIZE


static uint64_t g_curr_vl_id;
static uint32_t g_curr_vl_offset;
static byte g_curr_vl_data[VL_FILE_MAX_SIZE];

static char g_filename[32];

uint64_t initVlFile() {
    uint64_t ret = g_curr_vl_id;

    sprintf(g_filename, "%s/%lx", VL_PATH, g_curr_vl_id);
    FILE* vl_file = fopen(g_filename, "wb");
    fwrite(g_curr_vl_data, sizeof(byte), VL_FILE_MAX_SIZE, vl_file);
    fclose(vl_file);

    g_curr_vl_id = getTimestamp();
    g_curr_vl_offset = 0;

    return ret;
}

Metadata* addVlEntry(WalEntry* wal_entry) {
    if (!g_curr_vl_id) 
        g_curr_vl_id = getTimestamp();

    uint32_t vl_size = sizeof(VlEntry) + wal_entry->value_size;
    if (vl_size + g_curr_vl_offset > VL_FILE_MAX_SIZE) 
        initVlFile();

    VlEntry* vl_entry = (VlEntry*) &g_curr_vl_data[g_curr_vl_offset];
    vl_entry->timestamp = wal_entry->timestamp;
    vl_entry->value_size = wal_entry->value_size;
    memcpy(vl_entry->value, getWalEntryValue(wal_entry), vl_entry->value_size);
    vl_entry->crc = getVlEntryCrc(vl_entry);

    Metadata* metadata = malloc(sizeof(Metadata) + wal_entry->key_size);
    metadata->vl_id = g_curr_vl_id;
    metadata->offset = g_curr_vl_offset;
    metadata->timestamp = wal_entry->timestamp;
    metadata->key_size = wal_entry->key_size;
    memcpy(metadata->key, getWalEntryKey(wal_entry), metadata->key_size);
    metadata->crc = getMetadataCrc(metadata);

    g_curr_vl_offset += vl_size;

    return metadata;
}

VlEntry* getVlEntry(Metadata* metadata) {
    VlEntry* ret = NULL;
    if (metadata->vl_id == g_curr_vl_id) {
        VlEntry* vl_entry = (VlEntry*) &g_curr_vl_data[metadata->offset];
        ret = malloc(getVlEntrySize(vl_entry));
        memcpy(ret, vl_entry, getVlEntrySize(vl_entry));
    } else {
        sprintf(g_filename, "%s/%lx", VL_PATH, metadata->vl_id);
        FILE* vl_file = fopen(g_filename, "rb");
        if (!vl_file)
            return NULL;
        fseek(vl_file, metadata->offset, SEEK_SET);

        VlEntry vl_header;
        fread(&vl_header, sizeof(VlEntry), 1, vl_file);
        ret = malloc(sizeof(VlEntry) + vl_header.value_size);
        *ret = vl_header;
        fread(ret->value, ret->value_size, 1, vl_file);

        fclose(vl_file);
    }
    assert(getVlEntryCrc(ret) == ret->crc);
    return ret;
}

void saveVlFile() {
    sprintf(g_filename, "%s/%lx", VL_PATH, g_curr_vl_id);
    FILE* vl_file = fopen(g_filename, "wb");
    fwrite(g_curr_vl_data, sizeof(byte), VL_FILE_MAX_SIZE, vl_file);
    fclose(vl_file);
}

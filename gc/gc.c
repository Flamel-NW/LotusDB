#include "gc.h"

#include "index.h"
#include "vl_entry.h"
#include "dirent.h"


static char g_filename[512];

void gc(BTree* b_tree) {
    uint64_t vl_id = initVlFile();
    for (uint32_t i = 0; i < b_tree->size; i++) {
        BTreeLeaf* b_tree_leaf = b_tree->leaves[i];
        loadBTreeLeaf(b_tree_leaf);
        for (uint32_t j = 0; j < b_tree_leaf->size; j++) {
            Metadata* old_meta = getMetadata(b_tree_leaf, j);
            assert(old_meta->crc == getMetadataCrc(old_meta));
            VlEntry* vl_entry = getVlEntry(old_meta);
            assert(old_meta->timestamp == vl_entry->timestamp);
            WalEntry* wal_entry = initWalEntry(old_meta->key, vl_entry->value);

            wal_entry->timestamp = old_meta->timestamp;
            wal_entry->crc = getWalEntryCrc(wal_entry);

            Metadata* new_meta = addVlEntry(wal_entry);
            *old_meta = *new_meta;

            free(vl_entry);
            free(new_meta);
        }
        saveBTreeLeaf(b_tree_leaf);
    }

    DIR* dir = opendir(VL_PATH);
    struct dirent* dirent;
    while ((dirent = readdir(dir)) != NULL) {
        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
            continue;   // 忽略 '.' 和 '..'
        if (strtoul(dirent->d_name, NULL, 16) < vl_id) {
            sprintf(g_filename, "%s/%s", VL_PATH, dirent->d_name);
            remove(g_filename);
        }
    }
}

void fakeGc(BTree* b_tree) {
    DIR* dir = opendir(VL_PATH);
    struct dirent* dirent;
    while ((dirent = readdir(dir)) != NULL) {
        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
            continue;   // 忽略 '.' 和 '..'

        sprintf(g_filename, "%s/%s", VL_PATH, dirent->d_name);
        FILE* fp = fopen(g_filename, "rb");
        char test_key[KEY_MAX_LEN];
        srand((uint32_t) time(NULL));
        while (!feof(fp)) {
            fread(test_key, 26, 1, fp);
            sprintf(test_key, "key: %d", rand() % PAGE_SIZE);
            Metadata* old_meta = malloc(sizeof(Metadata) + strlen(test_key) + 1);
            getBTree(b_tree, test_key, old_meta);

            VlEntry* vl_entry;
            if ((vl_entry = getVlEntry(old_meta))) {
                WalEntry* wal_entry = initWalEntry(old_meta->key, vl_entry->value);

                wal_entry->timestamp = old_meta->timestamp;
                wal_entry->crc = getWalEntryCrc(wal_entry);

                Metadata* new_meta = addVlEntry(wal_entry);
                addBTree(b_tree, new_meta);

                free(vl_entry);
                free(old_meta);
                free(new_meta);
            }
        }
    }
}

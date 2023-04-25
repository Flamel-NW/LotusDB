#ifndef INDEX_H
#define INDEX_H

#include "pch.h"


// The Metadata looks like:
// +-------+-------+--------+-----------+----------+-------+
// |  crc  | vl_id | offset | timestamp | key_size |  key  |
// +-------+-------+--------+-----------+----------+-------+
// |---4---|---8---|---4----|-----8-----|----4-----|
// |--------------------header---------------------|
//         |-------------------crc check-------------------|

#pragma pack(4)
typedef struct metadata {
    uint32_t crc;
    uint64_t vl_id;
    uint32_t offset;

    uint64_t timestamp;
    uint32_t key_size;
    char key[];
} Metadata;
#pragma pack()

static inline uint32_t getMetadataSize(Metadata* metadata) {
    return sizeof(Metadata) + metadata->key_size;
}

static inline uint32_t getMetadataCrc(Metadata* metadata) {
    return crc32(((const byte*) metadata) + CRC_SHIFT, 
        getMetadataSize(metadata) - CRC_SHIFT);
}

static inline bool checkMetadata(Metadata* metadata) {
    if (!metadata->crc || !metadata->vl_id || !metadata->timestamp)
        return false;
    if (metadata->key_size <= 0 || metadata->key_size > KEY_MAX_LEN
            || metadata->key_size != strlen(metadata->key) + 1)
        return false;
    return metadata->crc == getMetadataCrc(metadata);
}

#define B_TREE_LEAF_SIZE    PAGE_SIZE
#define B_TREE_MAX_LEAVES   PAGE_SIZE

typedef struct b_tree_leaf {
    uint64_t file_id;
    char* first_key;

    uint32_t size;
    // 下标 0 ~ size-1 是size个Metadata的offsets, 下标 size 是末尾的offsets
    uint32_t offsets[B_TREE_LEAF_SIZE / sizeof(Metadata)];

    // Metadata为含有柔性数组成员的结构体 需要通过offsets灵活定位
    // fd为-1, metadata为NULL 代表未映射文件
    int32_t fd;
    byte* metadata;
} BTreeLeaf;

typedef struct b_tree {
    uint32_t size;              // 叶子数量
    uint32_t curr_leaf;         // 当前打开的叶子下标
    BTreeLeaf* leaves[B_TREE_MAX_LEAVES];
} BTree;


BTree* initBTree();

void addBTree(BTree* b_tree, Metadata* metadata);

// 因为源数据随时有可能被交换回磁盘 所以会被拷贝进ret中返回
// 成功会返回true, 失败返回false
bool getBTree(BTree* b_tree, const char* key, Metadata* ret);

bool removeBTree(BTree* b_tree, const char* key);

void delBTree(BTree* b_tree);

#endif

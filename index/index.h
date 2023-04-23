#ifndef INDEX_H
#define INDEX_H

#include "pch.h"


// The Metadata looks like:
// +-------+-------+--------+-----------+----------+-------+
// |  crc  | vl_id | offset | timestamp | key_size |  key  |
// +-------+-------+--------+-----------+----------+-------+
// |---4---|---4---|---4----|-----4-----|----8-----|
// |--------------------header---------------------|
//         |-------------------crc check-------------------|

#pragma pack(4)
typedef struct metadata {
    uint32_t crc;
    uint32_t vl_id;
    int32_t offset;

    int32_t timestamp;
    size_t key_size;
    char key[];
} Metadata;
#pragma pack()

static inline size_t getMetadataSize(Metadata* metadata) {
    return sizeof(Metadata) + metadata->key_size;
}

static inline uint32_t getMetadataCrc(Metadata* metadata) {
    return crc32((const byte*) metadata, getMetadataSize(metadata));
}

#define B_TREE_LEAF_SIZE    PAGE_SIZE
#define B_TREE_MAX_LEAVES   PAGE_SIZE

typedef struct b_tree_leaf {
    int32_t file_id;
    char* first_key;

    size_t size;
    // 下标 0 ~ size-1 是size个Metadata的offsets, 下标 size 是末尾的offsets
    uint32_t offsets[B_TREE_LEAF_SIZE / sizeof(Metadata)];

    // Metadata为含有柔性数组成员的结构体 需要通过offsets灵活定位
    // fd为-1, metadata为NULL 代表未映射文件
    int32_t fd;
    byte* metadata;
} BTreeLeaf;

typedef struct b_tree {
    size_t size;                // 叶子数量
    uint32_t curr_leaf;         // 当前打开的叶子下标
    BTreeLeaf* leaves[B_TREE_MAX_LEAVES];
} BTree;


BTree* initBTree();

void addBTree(BTree* b_tree, Metadata* metadata);

// 因为源数据随时有可能被交换回磁盘 所以会被拷贝进ret中返回
// 成功会返回true, 失败返回false
bool getBTree(BTree* b_tree, const char* key, Metadata* ret);

bool removeBTree(BTree* b_tree, const char* key);

#endif

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "pch.h"

#include "wal_entry.h"
#include "index.h"

typedef struct skip_list_node {
    WalEntry* wal_entry;

    struct skip_list_level {
        struct skip_list_node* next;
    } levels[];
} SkipListNode;

typedef struct skip_list {
    SkipListNode* head;

    uint32_t length;
    uint32_t level;
} SkipList;

SkipListNode* initSkipListNode();

SkipList* initSkipList();

// 添加 & 修改
void addSkipList(SkipList* skip_list, WalEntry* wal_entry);

WalEntry* getSkipList(SkipList* skip_list, const char* key);

void traverseSkipList(SkipList* skip_list, void (*func) (SkipListNode* skip_list_node));

void delSkipList(SkipList* skip_list);

// 清空SkipList 并将其中的WalEntry写入VL 和 BTree
void flushSkipList(SkipList* skip_list, BTree* b_tree);

#endif
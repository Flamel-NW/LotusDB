#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "pch.h"

#include "wal_entry.h"

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

void addSkipList(SkipList* skip_list, WalEntry* wal_entry);

WalEntry* getSkipList(SkipList* skip_list, const char* key);

void delSkipList(SkipList* skip_list);

#endif
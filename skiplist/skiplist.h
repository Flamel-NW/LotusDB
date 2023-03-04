#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <stdint.h>
#include <stdlib.h>

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

void addWalEntryToSkipList(SkipList* skip_list, WalEntry* wal_entry);

WalEntry* getWalEntryFromSkipList(SkipList* skip_list, const char* key);

#endif
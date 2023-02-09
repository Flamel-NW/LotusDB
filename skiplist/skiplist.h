#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <stdint.h>
#include <stdlib.h>

#include "pch.h"

typedef struct skip_list_node {

    uint32_t key_size;
    char key[KEY_MAX_LEN];

    uint32_t value_size;
    char value[VALUE_MAX_LEN];

    struct skip_list_level {
        struct skip_list_node* next;
    } levels[];
    
} SkipListNode;

typedef struct skip_list {
    SkipListNode* head;
    uint32_t length;
    uint32_t level;
} SkipList;

#endif
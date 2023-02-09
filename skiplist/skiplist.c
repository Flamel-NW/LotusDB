#include "skiplist.h"

const uint32_t MAX_LEVEL = 32;

SkipListNode* initSkipListNode(uint32_t level, Key key, Value value) {
    SkipListNode* skip_list_node = (SkipListNode*) malloc(sizeof(SkipListNode) + 
        sizeof(struct skip_list_level) * level);
    skip_list_node->key = key;
    skip_list_node->value = value;
    return skip_list_node;
}

SkipList* initSkipList() {
    SkipList* skip_list = (SkipList*) malloc(sizeof(SkipList));
    skip_list->length = 0;
    skip_list->level = 1;
    skip_list->head = initSkipListNode(MAX_LEVEL, KEY_MIN, 0);
    for (uint32_t i = 0; i < MAX_LEVEL; i++)
        skip_list->head->levels[i].next = NULL;
    return skip_list;
}



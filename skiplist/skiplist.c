#include "skiplist.h"
#include "pch.h"
#include "wal_entry.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const uint32_t SKIP_LIST_MAX_LEVEL = 32;

uint32_t getSkipListNodeRandomLevel() {
    uint32_t level = 1;
    while (rand() & 0x1) 				        // 抛硬币思想，随机数为奇数的概率可认为是1/2
        ++level;
    return min(level, SKIP_LIST_MAX_LEVEL); 	// 返回的最大层数不超过32
}

SkipList* initSkipList() {
    SkipList* skip_list = (SkipList*) malloc(sizeof(SkipList));
    skip_list->length = 0;
    skip_list->level = 1;
    
    WalEntry* wal_entry = (WalEntry*) malloc(sizeof(WalEntry));
    bzero(&(wal_entry->header), sizeof(WalHeader));
    wal_entry->key = strdup("head key");
    wal_entry->value = strdup("head value");
    skip_list->head = initSkipListNode(SKIP_LIST_MAX_LEVEL, wal_entry);

    for (int32_t i = 0; i < SKIP_LIST_MAX_LEVEL; i++)
        skip_list->head->levels[i].next = NULL;
    return skip_list;
}

SkipListNode* initSkipListNode(uint32_t level, WalEntry* wal_entry) {
    SkipListNode* skip_list_node = (SkipListNode*) 
        malloc(sizeof(SkipListNode) + sizeof(struct skip_list_level) * level);
    skip_list_node->wal_entry = wal_entry;
    return skip_list_node;
}

void addWalEntryToSkipList(SkipList* skip_list, WalEntry* wal_entry) {
    SkipListNode *p = skip_list->head;
    int32_t levelIdx = skip_list->level - 1;
    SkipListNode* pre_nodes[SKIP_LIST_MAX_LEVEL];   // 保存待插入节点的所有前驱节点的值
    for (int32_t i = skip_list->level; i < SKIP_LIST_MAX_LEVEL; ++i) 
        pre_nodes[i] = skip_list->head;			    // 初始化值为附加头结点

    for (int32_t i = levelIdx; i >= 0; --i) {
        // 如果第i层节点值小于target, 沿当前层继续查找插入的位置
        while( p->levels[i].next && strncmp(p->levels[i].next->wal_entry->key,
                 wal_entry->key, KEY_MAX_LEN) < 0) 
            p = p->levels[i].next;
        pre_nodes[i] = p;
    }

    uint32_t level = getSkipListNodeRandomLevel();	// 计算新插入节点的层数
    SkipListNode* newNode = initSkipListNode(level, wal_entry);
    for (int32_t i = 0; i < level; ++i) {
        newNode->levels[i].next = pre_nodes[i]->levels[i].next;
        pre_nodes[i]->levels[i].next = newNode;
    }
    skip_list->level = max(skip_list->level, level);    // 完成插入动作后，更新跳跃表当前层数
    ++skip_list->length;								// 完成插入动作后，更新跳跃表长度
}

WalEntry* getWalEntryFromSkipList(SkipList* skip_list, const char* key) {
    SkipListNode* p = skip_list->head;
    int32_t index = skip_list->level - 1;
    for (int32_t i = index; i >= 0; i--) {
        // 如果第i层节点值小于target, 就沿着当前层继续查找
        while (p->levels[i].next && 
                strncmp(p->levels[i].next->wal_entry->key, key, KEY_MAX_LEN) < 0) 
            p = p->levels[i].next;

        // 第i层未找到该节点, 或者节点值已大于target, 沿着下一层继续查找
        if (!(p->levels[i].next) || 
                strncmp(p->levels[i].next->wal_entry->key, key, KEY_MAX_LEN) > 0) 
            continue;
        
        return p->levels[i].next->wal_entry;
    }
    return NULL;
}



#include "memtable.h"
#include "pch.h"
#include "skiplist.h"
#include "wal_entry.h"



Memtable* initMemtable() {
    Memtable* memtable = (Memtable*) malloc(sizeof(Memtable));
    memtable->skip_list = initSkipList();
    pthread_rwlock_init(&memtable->rwlock, NULL);
    memtable->fp = fopen(WAL_NAME, "rb");
    if (memtable->fp) {

        pthread_rwlock_rdlock(&memtable->rwlock);
        WalEntry* wal_entry = loadWalEntry(memtable->fp);
        while (wal_entry) {
            addSkipList(memtable->skip_list, wal_entry);
            memtable->size += getWalEntrySize(wal_entry);
            wal_entry = loadWalEntry(memtable->fp);
        }
        pthread_rwlock_unlock(&memtable->rwlock);

        fclose(memtable->fp);
        memtable->fp = fopen(WAL_NAME, "ab");
    } else {
        memtable->fp = fopen(WAL_NAME, "wb");
    }
    return memtable;
}

void addMemtable(Memtable* memtable, WalEntry* wal_entry) {
    size_t wal_entry_size = getWalEntrySize(wal_entry);
    if (memtable->size + wal_entry_size > PAGE_SIZE) {
        //TODO:
    } else {
        pthread_rwlock_wrlock(&memtable->rwlock);
        addSkipList(memtable->skip_list, wal_entry);
        memtable->size += wal_entry_size;
        saveWalEntry(wal_entry, memtable->fp);
        pthread_rwlock_unlock(&memtable->rwlock);
    }
}

void delMemtable(Memtable* memtable) {
    delSkipList(memtable->skip_list);
    pthread_rwlock_destroy(&memtable->rwlock);
    fclose(memtable->fp);
}


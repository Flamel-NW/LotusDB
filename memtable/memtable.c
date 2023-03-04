#include "memtable.h"
#include "pch.h"
#include "skiplist.h"
#include "wal_entry.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

Memtable* initMemtable() {
    Memtable* memtable = (Memtable*) malloc(sizeof(Memtable));
    memtable->skip_list = initSkipList();
    pthread_rwlock_init(&memtable->rwlock, NULL);

    char timestamp[FILENAME_MAX_LEN];
    sprintf(timestamp, "%s/%ld", WAL_PATH, getTimestamp());

    // TODO:
    // memtable->fp = fopen(timestamp, "w");

    return memtable;
}

Memtable* initMemtableFromWal(FILE* wal) {
    Memtable* memtable = (Memtable*) malloc(sizeof(Memtable));
    memtable->skip_list = initSkipList();
    pthread_rwlock_init(&memtable->rwlock, NULL);

    pthread_rwlock_wrlock(&memtable->rwlock);
    memtable->fp = wal;
    WalEntry* wal_entry;
    while ((wal_entry = readWalEntryFromWal(memtable->fp))) 
        addWalEntryToSkipList(memtable->skip_list, wal_entry);
    pthread_rwlock_unlock(&memtable->rwlock);

    return memtable;
}



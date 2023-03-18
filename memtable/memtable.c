#include "memtable.h"
#include "pch.h"
#include "skiplist.h"
#include "wal_entry.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

Memtable* initMemtable() {
    Memtable* memtable = (Memtable*) malloc(sizeof(Memtable));
    memtable->skip_list = initSkipList();
    pthread_rwlock_init(&memtable->rwlock, NULL);
    memtable->fp = fopen(WAL_NAME, "r");
    if (memtable->fp) {
        readSkipListFromWal(memtable->skip_list, memtable->fp);
        fclose(memtable->fp);
        memtable->fp = fopen(WAL_NAME, "a");
    } else {
        memtable->fp = fopen(WAL_NAME, "w");
    }
    return memtable;
}

void delMemtable(Memtable* memtable) {
    writeSkipListToWal(memtable->skip_list, memtable->fp);
    delSkipList(memtable->skip_list);

    pthread_rwlock_destroy(&memtable->rwlock);
    fclose(memtable->fp);
}

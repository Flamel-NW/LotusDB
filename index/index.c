#include "index.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

static char g_filename[32];

static void loadBTreeLeaf(BTreeLeaf* b_tree_leaf) {
    if (b_tree_leaf->fd == -1 && !(b_tree_leaf->metadata)) {
        sprintf(g_filename, "%s/%lx", INDEX_PATH, b_tree_leaf->file_id);
        b_tree_leaf->fd = open(g_filename, O_RDWR);
        b_tree_leaf->metadata = 
            mmap(NULL, B_TREE_LEAF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, b_tree_leaf->fd, 0);
    }
}

static void saveBTreeLeaf(BTreeLeaf* b_tree_leaf) {
    if (b_tree_leaf->fd != -1 && b_tree_leaf->metadata) {
        close(b_tree_leaf->fd);
        b_tree_leaf->fd = -1;

        munmap(b_tree_leaf->metadata, B_TREE_LEAF_SIZE);
        b_tree_leaf->metadata = NULL;
    }
}

// 获取BTreeLeaf中的第n个Metadata
static inline Metadata* getMetadata(BTreeLeaf* b_tree_leaf, size_t n) {
    assert(n >= 0 && n <= b_tree_leaf->size);
    return (Metadata*) &b_tree_leaf->metadata[b_tree_leaf->offsets[n]];
}

// debug
static void printBTreeLeaf(BTreeLeaf* b_tree_leaf) {
    STDERR_FUNC_LINE();
    STDERR("file_id: %lx", b_tree_leaf->file_id);
    if (b_tree_leaf->first_key)
        STDERR("first_key: %s", b_tree_leaf->first_key);
    STDERR("size: %ld", b_tree_leaf->size);
    STDERR("end offset: %lu", b_tree_leaf->offsets[b_tree_leaf->size]);
    if (b_tree_leaf->fd == -1 && !b_tree_leaf->metadata) {
        STDERR("not loaded");
    } else {
        for (int32_t i = 0; i < b_tree_leaf->size; i++) {
            STDERR("No %d offset: %lu", i, b_tree_leaf->offsets[i]);
            STDERR("metadata key: %s\n", getMetadata(b_tree_leaf, i)->key);
            char* key = getMetadata(b_tree_leaf, i)->key;
            assert(key[0] == 'k' && key[1] == 'e' && key[2] == 'y' && key[3] == ':' && key[4] == ' ');
        }
    }
}

static BTreeLeaf* initBTreeLeaf() {
    BTreeLeaf* b_tree_leaf = malloc(sizeof(BTreeLeaf));

    b_tree_leaf->file_id = getTimestamp();
    b_tree_leaf->first_key = NULL;

    b_tree_leaf->size = 0;
    memset(b_tree_leaf->offsets, 0, sizeof(b_tree_leaf->offsets));

    sprintf(g_filename, "%s/%lx", INDEX_PATH, b_tree_leaf->file_id);
    b_tree_leaf->fd = open(g_filename, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    ftruncate(b_tree_leaf->fd, B_TREE_LEAF_SIZE);

    b_tree_leaf->metadata = 
        mmap(NULL, B_TREE_LEAF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, b_tree_leaf->fd, 0);

    return b_tree_leaf;
}

static BTreeLeaf* initBTreeLeafFromFile(uint32_t file_id) {
    BTreeLeaf* b_tree_leaf = malloc(sizeof(BTreeLeaf));

    b_tree_leaf->file_id = file_id;

    b_tree_leaf->size = 0;
    memset(b_tree_leaf->offsets, 0, sizeof(b_tree_leaf->offsets));

    b_tree_leaf->fd = -1;
    b_tree_leaf->metadata = NULL;

    loadBTreeLeaf(b_tree_leaf);

    while (true) {
        Metadata* metadata = getMetadata(b_tree_leaf, b_tree_leaf->size);
        if (metadata->crc == getMetadataCrc(metadata)) 
            b_tree_leaf->offsets[++b_tree_leaf->size] = getMetadataSize(metadata);
        else
            break;
    }      
    b_tree_leaf->first_key = strdup(getMetadata(b_tree_leaf, 0)->key);

    return b_tree_leaf;
}

// 默认调用了这个函数的b_tree_leaf 就是已经load了的
static void addBTreeLeaf(BTreeLeaf* b_tree_leaf, Metadata* metadata) {
    size_t meta_size = getMetadataSize(metadata);

    if (!b_tree_leaf->size && !b_tree_leaf->first_key) {
        b_tree_leaf->first_key = strdup(metadata->key);
        memcpy(b_tree_leaf->metadata, metadata, meta_size);
        b_tree_leaf->offsets[++b_tree_leaf->size] = meta_size;
    } else {
        int32_t l = 0;
        int32_t r = b_tree_leaf->size - 1;
        while (l <= r) {
            int32_t m = (l + r) >> 1;
            Metadata* mid = getMetadata(b_tree_leaf, m);
            if (strcmp(mid->key, metadata->key) < 0) {
                l = m + 1;
            } else if (strcmp(mid->key, metadata->key) > 0) {
                r = m - 1;
            } else {
                memcpy(mid, metadata, sizeof(Metadata));
                msync(b_tree_leaf->metadata, B_TREE_LEAF_SIZE, MS_ASYNC);
                return;
            }
        }

        if (r == -1) {
            if (b_tree_leaf->first_key)
                free(b_tree_leaf->first_key);
            b_tree_leaf->first_key = strdup(metadata->key);
        }

        byte* next = (byte*) getMetadata(b_tree_leaf, l);
        Metadata* debug = getMetadata(b_tree_leaf, l);
        memmove(next + meta_size, next, 
            b_tree_leaf->offsets[b_tree_leaf->size] - b_tree_leaf->offsets[l]);
        memcpy(next, metadata, meta_size);

        for (int32_t i = b_tree_leaf->size++; i > r; i--) 
            b_tree_leaf->offsets[i + 1] = b_tree_leaf->offsets[i] + meta_size;
    }
    msync(b_tree_leaf->metadata, B_TREE_LEAF_SIZE, MS_ASYNC);
}

static bool getBTreeLeaf(BTreeLeaf* b_tree_leaf, const char* key, Metadata* ret) {
    int32_t l = 0;
    int32_t r = b_tree_leaf->size - 1;
    while (l <= r) {
        int32_t m = (l + r) >> 1;
        Metadata* mid = getMetadata(b_tree_leaf, m);
        if (strcmp(mid->key, key) < 0) {
            l = m + 1;
        }
        else if (strcmp(mid->key, key) > 0) {
            r = m - 1;
        }
        else {
            assert(getMetadataCrc(mid) == mid->crc);
            memcpy(ret, mid, getMetadataSize(mid));
            return true;
        }
    }
    return false;
}

static bool removeBTreeLeaf(BTreeLeaf* b_tree_leaf, const char* key) {
    int32_t l = 0;
    int32_t r = b_tree_leaf->size - 1;
    while (l <= r) {
        int32_t m = (l + r) >> 1;
        Metadata* mid = getMetadata(b_tree_leaf, m);
        if (strcmp(mid->key, key) < 0) {
            l = m + 1;
        }
        else if (strcmp(mid->key, key) > 0) {
            r = m - 1;
        }
        else {
            size_t meta_size = getMetadataSize(mid);
            memmove(mid, getMetadata(b_tree_leaf, m + 1), 
                b_tree_leaf->offsets[b_tree_leaf->size] - b_tree_leaf->offsets[m + 1]);
            for (int32_t i = m; i < b_tree_leaf->size; i++)
                b_tree_leaf->offsets[i] = b_tree_leaf->offsets[i + 1] - meta_size;
            b_tree_leaf->size--;
        }
    }
    return false;
}

static void delBTreeLeaf(BTreeLeaf* b_tree_leaf) {
    saveBTreeLeaf(b_tree_leaf);
    if (b_tree_leaf->first_key)
        free(b_tree_leaf->first_key);
    free(b_tree_leaf);
}

static inline BTreeLeaf* getCurrLeaf(BTree* b_tree) {
    return b_tree->leaves[b_tree->curr_leaf];
}

static BTreeLeaf* switchCurrLeaf(BTree* b_tree, const char* key) {
    if (getCurrLeaf(b_tree)->first_key &&
        (!(strcmp(getCurrLeaf(b_tree)->first_key, key) <= 0 && 
            (b_tree->curr_leaf == b_tree->size - 1 || 
                strcmp(b_tree->leaves[b_tree->curr_leaf + 1]->first_key, key) > 0)))) {
        saveBTreeLeaf(getCurrLeaf(b_tree));

        int32_t l = 0;
        int32_t r = b_tree->size - 1;
        while (l <= r) {
            int32_t m = (l + r) >> 1;
            if (strcmp(b_tree->leaves[m]->first_key, key) > 0)
                r = m - 1;
            else
                l = m + 1;
        }
        b_tree->curr_leaf = r;

        loadBTreeLeaf(getCurrLeaf(b_tree));
    }
    return getCurrLeaf(b_tree);
}


static void splitCurrLeaf(BTree* b_tree) {
    STDERR_FUNC_LINE();

    BTreeLeaf* curr_leaf = getCurrLeaf(b_tree);
    BTreeLeaf* new_leaf = initBTreeLeaf();
    BTreeLeaf* next_leaf = b_tree->leaves[b_tree->curr_leaf + 1];

    for (int32_t i = b_tree->size++ - 1; i > b_tree->curr_leaf; i--) 
        b_tree->leaves[i + 1] = b_tree->leaves[i];
    b_tree->leaves[b_tree->curr_leaf + 1] = new_leaf;

    // 迁移第 m + 1 之后的Metadata
    int32_t m = curr_leaf->size >> 1;
    memmove(getMetadata(new_leaf, 0), getMetadata(curr_leaf, m + 1), 
        curr_leaf->offsets[curr_leaf->size] - curr_leaf->offsets[m + 1]);

    new_leaf->size = curr_leaf->size - m - 1;

    int32_t i = m + 1;
    int32_t j = 0;
    while (i <= curr_leaf->size) {
        new_leaf->offsets[j++] = curr_leaf->offsets[i++] - curr_leaf->offsets[m + 1];
        STDERR("curr metadata key:%s", getMetadata(curr_leaf, i - 1)->key);
        STDERR("new metadata key:%s", getMetadata(new_leaf, j - 1)->key);
    }

    curr_leaf->size = m + 1;

    new_leaf->first_key = strdup(getMetadata(new_leaf, 0)->key);

    saveBTreeLeaf(new_leaf);
}

static void mergeCurrLeaf(BTree* b_tree) {
    BTreeLeaf* curr_leaf = getCurrLeaf(b_tree);
    BTreeLeaf* del_leaf = b_tree->leaves[b_tree->curr_leaf + 1];
    BTreeLeaf* next_leaf = b_tree->leaves[b_tree->curr_leaf + 2];

    loadBTreeLeaf(del_leaf);

    b_tree->size--;
    for (int32_t i = b_tree->curr_leaf + 1; i < b_tree->size; i++)
        b_tree->leaves[i] = b_tree->leaves[i + 1];

    memmove(getMetadata(curr_leaf, curr_leaf->size), del_leaf->metadata,
        del_leaf->offsets[del_leaf->size]);

    int32_t i = 0;
    int32_t j = curr_leaf->size;
    while (i <= del_leaf->size)
        curr_leaf->offsets[j++] = del_leaf->offsets[i++] + curr_leaf->offsets[curr_leaf->size];
    curr_leaf->size += del_leaf->size;

    delBTreeLeaf(del_leaf);
}


BTree* initBTree() {
    BTree* b_tree = malloc(sizeof(BTree) + sizeof(BTreeLeaf*) * B_TREE_MAX_LEAVES);
    b_tree->size = 0;

    DIR* dir = opendir(INDEX_PATH);
    struct dirent* dirent;
    while ((dirent = readdir(dir)) != NULL) {
        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
            continue;   // 忽略 '.' 和 '..'

        BTreeLeaf* b_tree_leaf = initBTreeLeafFromFile(strtoul(dirent->d_name, NULL, 16));
        if (!b_tree->size) {
            b_tree->leaves[b_tree->size++] = b_tree_leaf;
            b_tree->curr_leaf = 0;
        } else {
            switchCurrLeaf(b_tree, b_tree_leaf->first_key);
            memmove(&b_tree->leaves[b_tree->curr_leaf + 2], &b_tree->leaves[b_tree->curr_leaf + 1], 
                sizeof(BTreeLeaf*) * (b_tree->size++ - b_tree->curr_leaf - 1));
            b_tree->leaves[b_tree->curr_leaf + 1] = b_tree_leaf;
            saveBTreeLeaf(b_tree_leaf);
        }
    }
    
    if (!b_tree->size) {
        b_tree->leaves[b_tree->size++] = initBTreeLeaf();
        b_tree->curr_leaf = 0;
    }

    return b_tree;
}

void addBTree(BTree* b_tree, Metadata* metadata) {
    BTreeLeaf* curr_leaf = switchCurrLeaf(b_tree, metadata->key);
    if (curr_leaf->offsets[curr_leaf->size] + 
            getMetadataSize(metadata) > B_TREE_LEAF_SIZE) {
        splitCurrLeaf(b_tree);
        curr_leaf = switchCurrLeaf(b_tree, metadata->key);
    }
    addBTreeLeaf(curr_leaf, metadata);
}

bool getBTree(BTree* b_tree, const char* key, Metadata* ret) {
    BTreeLeaf* curr_leaf = switchCurrLeaf(b_tree, key);
    return getBTreeLeaf(curr_leaf, key, ret);
}

bool removeBTree(BTree* b_tree, const char* key) {
    BTreeLeaf* curr_leaf = switchCurrLeaf(b_tree, key);
    bool ret = removeBTreeLeaf(curr_leaf, key);

    if (curr_leaf->offsets[curr_leaf->size] < B_TREE_LEAF_SIZE >> 1) {
        BTreeLeaf* prev_leaf = b_tree->curr_leaf > 0 ? 
            b_tree->leaves[b_tree->curr_leaf - 1] : NULL;
        BTreeLeaf* next_leaf = b_tree->curr_leaf < b_tree->size - 1 ? 
            b_tree->leaves[b_tree->curr_leaf + 1] : NULL;
        if (prev_leaf && curr_leaf->offsets[curr_leaf->size] + 
                prev_leaf->offsets[prev_leaf->size] < B_TREE_LEAF_SIZE) {
            b_tree->curr_leaf--;
            mergeCurrLeaf(b_tree);
        } else if (next_leaf && curr_leaf->offsets[curr_leaf->size] + 
                next_leaf->offsets[next_leaf->size] < B_TREE_LEAF_SIZE) {
            mergeCurrLeaf(b_tree);
        }
    }

    return ret;
}

void delBTree(BTree* b_tree) {
    for (int32_t i = 0; i < b_tree->size; i++)
        delBTreeLeaf(b_tree->leaves[i]);
    free(b_tree);
}

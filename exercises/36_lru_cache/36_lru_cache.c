/*
 * === LRU 缓存淘汰算法 ===
 *
 * LRU (Least Recently Used) 是一种常用的缓存淘汰策略
 * 核心思想：当缓存满时，淘汰最久未被访问的数据
 *
 * 数据结构组合：哈希表 + 双向链表
 * - 哈希表：O(1) 时间查找键值对
 * - 双向链表：维护访问顺序，头部是最近使用，尾部是最久未使用
 *
 * 操作复杂度：
 * - get(k): O(1) - 哈希表查找 + 链表节点移动
 * - put(k,v): O(1) - 哈希表插入 + 链表节点插入/删除
 *
 * 工作原理：
 * 1. 访问 (get) 数据时，将对应节点移到链表头部
 * 2. 插入 (put) 数据时，新节点直接放到链表头部
 * 3. 容量满时，删除链表尾部节点 (最久未使用)
 *
 * 链表示意图：
 *   [MRU] <-> [节点2] <-> [节点3] <-> [LRU]
 *    最近使用                          最久未使用
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 数据结构定义
 */

typedef struct LRUNode {
    int key;
    int value;
    struct LRUNode* prev;
    struct LRUNode* next;
} LRUNode;

typedef struct HashEntry {
    int key;
    LRUNode* node;
    struct HashEntry* next;
} HashEntry;

typedef struct {
    int capacity;
    int size;
    LRUNode* head; /* 最近使用（MRU） */
    LRUNode* tail; /* 最久未使用（LRU） */
    /* 简单链式哈希表 */
    size_t bucket_count;
    HashEntry** buckets;
} LRUCache;

/**
 * 整数哈希函数
 *
 * @param key 要哈希的整数键
 * @return 哈希值 (非负整数)
 */
static unsigned hash_int(int key) {
    return (unsigned)(key < 0 ? -key : key);
}

/**
 * 在哈希表中查找键
 *
 * @param c LRU 缓存指针
 * @param key 要查找的键
 * @param pprev_next 输出参数，返回找到节点的父指针的地址
 * @return 找到的哈希表项指针，未找到返回 NULL
 *
 * @note pprev_next 用于删除操作，可以 O(1) 时间从链表中移除节点
 */
static HashEntry* hash_find(LRUCache* c, int key, HashEntry*** pprev_next) {
    unsigned idx = hash_int(key) % c->bucket_count;
    HashEntry** pprev = &c->buckets[idx];
    HashEntry* entry = *pprev;

    while (entry) {
        if (entry->key == key) {
            *pprev_next = pprev;
            return entry;
        }
        pprev = &entry->next;
        entry = entry->next;
    }

    *pprev_next = pprev;
    return NULL;
}

/**
 * 将节点添加到链表头部 (标记为最近使用)
 *
 * @param c LRU 缓存指针
 * @param node 要添加的节点
 */
static void list_add_to_head(LRUCache* c, LRUNode* node) {
    node->prev = NULL;
    node->next = c->head;

    if (c->head) {
        c->head->prev = node;
    }
    c->head = node;

    if (!c->tail) {
        c->tail = node;
    }
}

/**
 * 从链表中移除节点
 *
 * @param c LRU 缓存指针
 * @param node 要移除的节点
 */
static void list_remove(LRUCache* c, LRUNode* node) {
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        c->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        c->tail = node->prev;
    }
}

/**
 * 将节点移动到链表头部 (标记为最近使用)
 *
 * @param c LRU 缓存指针
 * @param node 要移动的节点
 */
static void list_move_to_head(LRUCache* c, LRUNode* node) {
    list_remove(c, node);
    list_add_to_head(c, node);
}

/**
 * 弹出链表尾部节点 (最久未使用)
 *
 * @param c LRU 缓存指针
 * @return 被弹出的节点指针，链表为空时返回 NULL
 */
static LRUNode* list_pop_tail(LRUCache* c) {
    if (!c->tail) {
        return NULL;
    }

    LRUNode* node = c->tail;
    list_remove(c, node);
    return node;
}

/* LRU 接口实现 */
static LRUCache* lru_create(int capacity) {
    LRUCache* c = (LRUCache*)calloc(1, sizeof(LRUCache));
    if (!c) {
        return NULL;
    }

    c->capacity = capacity;
    c->size = 0;
    c->head = NULL;
    c->tail = NULL;
    c->bucket_count = 16;

    c->buckets = (HashEntry**)calloc(c->bucket_count, sizeof(HashEntry*));
    if (!c->buckets) {
        free(c);
        return NULL;
    }

    return c;
}

static void lru_free(LRUCache* c) {
    if (!c) {
        return;
    }

    LRUNode* node = c->head;
    while (node) {
        LRUNode* next = node->next;
        free(node);
        node = next;
    }

    for (size_t i = 0; i < c->bucket_count; i++) {
        HashEntry* entry = c->buckets[i];
        while (entry) {
            HashEntry* next = entry->next;
            free(entry);
            entry = next;
        }
    }

    free(c->buckets);
    free(c);
}

static int lru_get(LRUCache* c, int key, int* out_value) {
    HashEntry** pprev;
    HashEntry* entry = hash_find(c, key, &pprev);

    if (!entry) {
        return 0;
    }

    list_move_to_head(c, entry->node);
    *out_value = entry->node->value;
    return 1;
}

static void lru_put(LRUCache* c, int key, int value) {
    HashEntry** pprev;
    HashEntry* entry = hash_find(c, key, &pprev);

    if (entry) {
        entry->node->value = value;
        list_move_to_head(c, entry->node);
        return;
    }

    LRUNode* node = (LRUNode*)malloc(sizeof(LRUNode));
    if (!node) {
        return;
    }

    node->key = key;
    node->value = value;

    if (c->size >= c->capacity) {
        LRUNode* lru = list_pop_tail(c);
        if (lru) {
            HashEntry** lru_pprev;
            HashEntry* lru_entry = hash_find(c, lru->key, &lru_pprev);
            if (lru_entry) {
                *lru_pprev = lru_entry->next;
                free(lru_entry);
            }
            free(lru);
            c->size--;
        }
    }

    list_add_to_head(c, node);
    c->size++;

    HashEntry* new_entry = (HashEntry*)malloc(sizeof(HashEntry));
    if (!new_entry) {
        list_remove(c, node);
        free(node);
        c->size--;
        return;
    }

    new_entry->key = key;
    new_entry->node = node;
    new_entry->next = c->buckets[hash_int(key) % c->bucket_count];
    c->buckets[hash_int(key) % c->bucket_count] = new_entry;
}

/* 打印当前缓存内容（从头到尾） */
static void lru_print(LRUCache* c) {
    LRUNode* p = c->head;
    int first = 1;
    while (p) {
        if (!first) printf(", ");
        first = 0;
        printf("%d:%d", p->key, p->value);
        p = p->next;
    }
    printf("\n");
}

int main(void) {
    /* 容量 3：put(1,1), put(2,2), put(3,3), put(4,4), get(2), put(5,5) */
    LRUCache* c = lru_create(3);
    if (!c) {
        fprintf(stderr, "创建 LRU 失败\n");
        return 1;
    }

    lru_put(c, 1, 1); /* 缓存：1 */
    lru_put(c, 2, 2); /* 缓存：2,1 */
    lru_put(c, 3, 3); /* 缓存：3,2,1 (满) */
    lru_put(c, 4, 4); /* 淘汰 LRU(1)，缓存：4,3,2 */

    int val;
    if (lru_get(c, 2, &val)) {
        /* 访问 2：缓存：2,4,3 */
        (void)val; /* 演示无需使用 */
    }

    lru_put(c, 5, 5); /* 淘汰 LRU(3)，缓存：5,2,4 */

    /* 期望最终键集合：{2,4,5}，顺序无关。此处按最近->最久打印：5:5, 2:2, 4:4 */
    lru_print(c);

    lru_free(c);
    return 0;
}

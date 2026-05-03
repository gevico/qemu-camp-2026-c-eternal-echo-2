/*
 * 哈希表词频统计工具
 *
 * 本程序统计文本文件中单词的出现频率，使用哈希表实现高效计数
 *
 * 哈希表设计：
 * - 采用链地址法解决冲突
 * - djb2 哈希算法
 * - 支持动态统计和排序输出
 *
 * 处理流程：
 * 1. 读取文件内容
 * 2. 逐词提取并插入哈希表
 * 3. 按频率排序输出结果
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 1024  // 哈希表桶的数量

/**
 * 哈希表节点结构
 *
 * 每个节点存储一个单词及其出现次数
 * 相同哈希值的节点通过链表连接
 */
typedef struct HashNode {
    char *word;              // 单词字符串 (动态分配)
    int count;               // 出现次数
    struct HashNode *next;   // 链表中的下一个节点
} HashNode;

/**
 * 哈希表结构
 *
 * 包含一个桶数组和表大小
 */
typedef struct {
    HashNode **table;        // 桶数组 (每个桶是一个链表头指针)
    int size;                // 哈希表大小 (桶的数量)
} HashTable;

/**
 * djb2 哈希函数
 *
 * 由 Daniel J. Bernstein 设计的经典字符串哈希算法
 *
 * @param str 要计算哈希值的字符串
 * @return 哈希值
 *
 * 算法：hash = hash * 33 + c
 * - 初始值：5381
 * - 乘数 33 的选择基于经验，具有较好的分布特性
 * - 可以用移位优化：hash << 5 + hash + c
 *
 * @note 此函数返回原始哈希值，使用时需要对表大小取模
 */
unsigned long djb2_hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/**
 * 创建哈希表
 *
 * @param size 哈希表大小 (桶的数量)
 * @return 新创建的哈希表指针
 *
 * 初始化：
 * - 分配哈希表结构体
 * - 分配桶数组并初始化为 NULL
 * - 保存表大小
 */
HashTable *create_hash_table(int size) {
    HashTable *ht = malloc(sizeof(HashTable));
    ht->table = calloc(size, sizeof(HashNode *));
    ht->size = size;
    return ht;
}

/**
 * 向哈希表插入单词或增加计数
 *
 * @param ht 哈希表指针
 * @param word 要插入的单词
 *
 * 操作步骤：
 * 1. 计算单词的哈希值
 * 2. 在对应桶的链表中查找单词
 * 3. 如果找到，增加计数
 * 4. 如果未找到，创建新节点并插入链表头部
 *
 * 时间复杂度：平均 O(1)，最坏 O(n)
 */
void hash_table_insert(HashTable *ht, const char *word) {
    unsigned long hash = djb2_hash(word) % ht->size;

    HashNode *node = ht->table[hash];
    while (node != NULL) {
        if (strcmp(node->word, word) == 0) {
            node->count++;
            return;
        }
        node = node->next;
    }

    HashNode *new_node = malloc(sizeof(HashNode));
    new_node->word = strdup(word);
    new_node->count = 1;
    new_node->next = ht->table[hash];
    ht->table[hash] = new_node;
}

/**
 * 收集哈希表中所有单词节点
 *
 * @param ht 哈希表指针
 * @param nodes 输出数组，存储所有节点的指针
 * @param count 输出参数，返回收集的节点数量
 *
 * @note 此函数用于后续的排序操作
 */
void get_all_words(HashTable *ht, HashNode **nodes, int *count) {
    *count = 0;
    for (int i = 0; i < ht->size; i++) {
        HashNode *node = ht->table[i];
        while (node != NULL) {
            nodes[(*count)++] = node;
            node = node->next;
        }
    }
}

/**
 * 节点比较函数 (用于 qsort)
 *
 * @param a 节点指针 A
 * @param b 节点指针 B
 * @return 比较结果
 *
 * 排序规则：
 * 1. 首先按频率降序排列
 * 2. 频率相同时按单词字母顺序排列
 */
int compare_nodes(const void *a, const void *b) {
    HashNode *node_a = *(HashNode **)a;
    HashNode *node_b = *(HashNode **)b;

    if (node_a->count != node_b->count) {
        return node_b->count - node_a->count;
    }
    return strcmp(node_a->word, node_b->word);
}

/**
 * 释放哈希表占用的所有内存
 *
 * @param ht 哈希表指针
 *
 * 释放顺序：
 * 1. 遍历所有桶
 * 2. 释放每个链表中的所有节点 (包括单词字符串)
 * 3. 释放桶数组
 * 4. 释放哈希表结构体
 */
void free_hash_table(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        HashNode *node = ht->table[i];
        while (node != NULL) {
            HashNode *temp = node;
            node = node->next;
            free(temp->word);
            free(temp);
        }
    }
    free(ht->table);
    free(ht);
}

/**
 * 从文本中提取下一个单词
 *
 * @param text 指向文本指针的指针 (会被更新)
 * @return 新分配的单词字符串，如果没有更多单词返回 NULL
 *
 * 单词定义：连续的字母字符序列
 *
 * @note 调用者需要负责释放返回的字符串
 */
char *get_next_word(const char **text) {
    while (**text && !isalpha(**text)) {
        (*text)++;
    }

    if (**text == '\0') {
        return NULL;
    }

    const char *start = *text;
    while (**text && isalpha(**text)) {
        (*text)++;
    }

    int len = *text - start;
    char *word = malloc(len + 1);
    strncpy(word, start, len);
    word[len] = '\0';

    return word;
}

/**
 * 主函数：词频统计程序
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return 程序退出状态码
 *
 * 处理流程：
 * 1. 打开文件 paper.txt
 * 2. 逐行读取并提取单词
 * 3. 将单词插入哈希表统计
 * 4. 收集所有单词节点
 * 5. 按频率排序
 * 6. 输出统计结果
 */
int main(int argc, char *argv[]) {
    const char* file_path = "paper.txt";

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("无法打开文件");
        return 1;
    }

    HashTable *ht = create_hash_table(TABLE_SIZE);
    char buffer[4096];

    printf("正在读取文件: %s\n", file_path);

    // 从文件读取直到EOF
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        const char *ptr = buffer;
        char *word;

        while ((word = get_next_word(&ptr)) != NULL) {
            hash_table_insert(ht, word);
            free(word);
        }
    }

    fclose(file);

    // 收集所有单词节点
    HashNode **nodes = malloc(TABLE_SIZE * sizeof(HashNode *));
    int node_count = 0;
    get_all_words(ht, nodes, &node_count);

    // 排序
    qsort(nodes, node_count, sizeof(HashNode *), compare_nodes);

    // 输出结果
    printf("\n单词统计结果（按频率降序排列）:\n");
    printf("%-20s %s\n", "单词", "出现次数");
    printf("-------------------- ----------\n");
    for (int i = 0; i < node_count; i++) {
        printf("%s:%d\n", nodes[i]->word, nodes[i]->count);
    }

    // 释放内存
    free(nodes);
    free_hash_table(ht);

    return 0;
}
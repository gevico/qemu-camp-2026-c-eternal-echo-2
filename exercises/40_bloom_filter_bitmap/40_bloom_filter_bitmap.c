/*
 * === Bloom 过滤器 ===
 *
 * Bloom Filter 是一种空间效率很高的概率型数据结构
 * 用于判断一个元素是否在集合中
 *
 * 特点：
 * - 可能的误判 (False Positive)：可能判断元素存在，实际不存在
 * - 无漏报 (No False Negative)：判断元素不存在，则一定不存在
 *
 * 核心参数：
 * - m: 位图大小 (比特数)
 * - k: 哈希函数个数
 *
 * 空间效率：
 * - 每个元素只需要 k 个比特
 * - 独立于元素本身的大小
 *
 * 应用场景：
 * - 数据库查询优化
 * - 网络爬虫 URL 去重
 * - 缓存系统
 * - 拼写检查
 *
 * 工作原理：
 * 1. 插入元素：对元素使用 k 个哈希函数，得到 k 个位置，将位图中对应位置置 1
 * 2. 查询元素：对元素使用 k 个哈希函数，检查对应位置是否都为 1
 *    - 如果所有位置都是 1，则元素可能存在
 *    - 如果有任一位置为 0，则元素一定不存在
 *
 * 误报率分析：
 * - 误报率随 k 和 m 的增加而降低
 * - 对于 m=100, k=3 的配置，误报率相对较高
 * - 实际应用中需要根据数据规模选择合适的参数
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Bloom 过滤器结构体
 *
 * @param bits 位图数组 (每个字节存储 8 比特)
 * @param m 位图大小 (比特数)
 */
typedef struct {
    unsigned char *bits;
    size_t m; /* 位图大小（比特） */
} Bloom;

/**
 * 创建 Bloom 过滤器
 *
 * @param m 位图大小 (比特数)
 * @return 新创建的 Bloom 过滤器指针，失败返回 NULL
 */
static Bloom *bloom_create(size_t m) {
    Bloom *bf = (Bloom *)malloc(sizeof(Bloom));
    if (!bf) {
        return NULL;
    }

    size_t bytes = (m + 7) / 8;
    bf->bits = (unsigned char *)calloc(bytes, sizeof(unsigned char));
    if (!bf->bits) {
        free(bf);
        return NULL;
    }

    bf->m = m;
    return bf;
}

/**
 * 释放 Bloom 过滤器
 *
 * @param bf Bloom 过滤器指针
 */
static void bloom_free(Bloom *bf) {
    if (bf) {
        free(bf->bits);
        free(bf);
    }
}

/*
 * === 位操作辅助函数 ===
 */

/**
 * 设置位图中指定位为 1
 *
 * @param bm 位图数组
 * @param idx 要设置的位索引
 *
 * 实现细节：
 * - byte = idx / 8: 定位到包含该位的字节
 * - offset = idx % 8: 定位到该字节内的偏移量
 * - 使用按或操作设置该位为 1
 */
static void set_bit(unsigned char *bm, size_t idx) {
    size_t byte = idx / 8;
    size_t offset = idx % 8;
    bm[byte] |= (1U << offset);
}

/**
 * 测试位图中指定位是否为 1
 *
 * @param bm 位图数组
 * @param idx 要测试的位索引
 * @return 如果该位为 1 返回 1，否则返回 0
 */
static int test_bit(const unsigned char *bm, size_t idx) {
    size_t byte = idx / 8;
    size_t offset = idx % 8;
    return (bm[byte] >> offset) & 1U;
}

/*
 * === 哈希函数 ===
 */

/**
 * 简单的字符串哈希函数
 *
 * @param s 要哈希的字符串
 * @param m 位图大小 (用于取模)
 * @param k 哈希函数编号 (1, 2, 3, ...)
 * @return 哈希值 (0 到 m-1)
 *
 * 算法：sum(c * k) % m
 * - 对每个字符乘以 k 参数累加
 * - 使用不同的 k 值得到不同的哈希函数
 *
 * @note 这是一个简化的哈希实现，实际应用中应使用更复杂的算法
 */
static size_t hash_k(const char *s, size_t m, int k) {
    size_t hash = 0;
    while (*s) {
        hash += (size_t)(*s * k);
        s++;
    }
    return hash % m;
}

/**
 * 向 Bloom 过滤器中插入元素
 *
 * @param bf Bloom 过滤器指针
 * @param s 要插入的字符串
 *
 * 操作步骤：
 * 1. 使用 k 个不同的哈希函数
 * 2. 将计算出的 k 个位置全部置 1
 */
static void bloom_add(Bloom *bf, const char *s) {
    for (int k = 1; k <= 3; k++) {
        size_t idx = hash_k(s, bf->m, k);
        set_bit(bf->bits, idx);
    }
}

/**
 * 查询元素是否可能存在于 Bloom 过滤器中
 *
 * @param bf Bloom 过滤器指针
 * @param s 要查询的字符串
 * @return 1 表示可能存在，0 表示一定不存在
 *
 * 查询逻辑：
 * 1. 使用 k 个哈希函数计算位置
 * 2. 如果所有对应位都是 1，则可能存在
 * 3. 如果有任一位是 0，则一定不存在
 *
 * @note 此函数可能产生误报 (False Positive)，但不会漏报
 */
static int bloom_maybe_contains(Bloom *bf, const char *s) {
    for (int k = 1; k <= 3; k++) {
        size_t idx = hash_k(s, bf->m, k);
        if (!test_bit(bf->bits, idx)) {
            return 0;
        }
    }
    return 1;
}

/**
 * 主函数：Bloom 过滤器演示
 *
 * @return 程序退出状态码
 *
 * 测试用例：
 * 1. 插入 "apple" 和 "banana"
 * 2. 查询 "apple"：应该返回 1 (无假阴性)
 * 3. 查询 "grape"：可能返回 1 (误报) 或 0
 */
int main(void) {
    const size_t m = 100; /* 位图大小 */
    Bloom *bf = bloom_create(m);
    if (!bf) {
        fprintf(stderr, "bloom create failed\n");
        return 1;
    }

    /* 插入元素："apple"、"banana" */
    bloom_add(bf, "apple");
    bloom_add(bf, "banana");

    /* 查询元素："apple"（应存在）、"grape"（可能存在误判） */
    int apple = bloom_maybe_contains(bf, "apple");
    int grape = bloom_maybe_contains(bf, "grape"); /* 未插入，可能误判 */

    printf("apple exists: %d\n", apple); /* 必须为 1（无假阴性） */
    printf("grape exists: %d\n", grape); /* 允许 0 或 1（可能误判） */

    bloom_free(bf);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 17 位图操作
 * 要求：实现 set_bit 与 test_bit，按位操作 (byte=bit/8, offset=bit%8)
 * 演示：位图大小 10（位），置位 0、3、5；查询 3→1，查询 1→0
 */

/* 将第 bit_index 位设置为 1（LSB 为最低位） */
static void set_bit(unsigned char* bitmap, size_t bit_index) {
    size_t byte = bit_index / 8;
    size_t offset = bit_index % 8;
    bitmap[byte] |= (1U << offset);
}

/* 读取第 bit_index 位，返回 0/1（LSB 为最低位） */
static int test_bit(const unsigned char* bitmap, size_t bit_index) {
    size_t byte = bit_index / 8;
    size_t offset = bit_index % 8;
    return (bitmap[byte] >> offset) & 1U;
}

int main(void) {
    /* 位图大小 10 位 → 需要 2 个字节 */
    const size_t bits = 10;
    const size_t bytes = (bits + 7) / 8; /* 向上取整 */
    unsigned char* bitmap = (unsigned char*)calloc(bytes, sizeof(unsigned char));
    if (!bitmap) {
        fprintf(stderr, "内存分配失败\n");
        return 1;
    }

    /* 置位 0、3、5 */
    set_bit(bitmap, 0);
    set_bit(bitmap, 3);
    set_bit(bitmap, 5);

    /* 查询并按题意输出 */
    int b3 = test_bit(bitmap, 3);
    int b1 = test_bit(bitmap, 1);
    printf("query(3)=%d\n", b3); /* 期望 1 */
    printf("query(1)=%d\n", b1); /* 期望 0 */

    free(bitmap);
    return 0;
}

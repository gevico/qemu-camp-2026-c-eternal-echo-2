#include <stddef.h>  // offsetof
#include <stdio.h>

/*
 * 12 container_of 宏实现
 * 要求：
 *  - 使用 GCC 扩展（typeof、语句表达式）实现 container_of(ptr, type, member)
 *  - 通过结构体成员的指针获取结构体变量的指针
 *  - 示例：struct Test { int a; char b; }，当 ptr=&t.b 时应返回 &t
 */

struct Test {
    int a;
    char b;
};

/*
 * container_of 宏
 * 原理：结构体首地址 + 成员偏移量 = 成员地址
 *      → 结构体首地址 = 成员地址 - 成员偏移量
 * 类型约束：使用 typeof 对 ptr 的类型进行校验，减少误用风险
 */
#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type, member) ); \
})

int main(void) {
    struct Test t = {.a = 42, .b = 'Z'};

    /* 取成员 b 的指针 */
    char *ptr_to_b = &t.b;

    /* 使用 container_of 通过成员指针反推结构体指针 */
    struct Test *owner = container_of(ptr_to_b, struct Test, b);

    /* 输出两个地址，应当一致（测试程序将解析并验证） */
    printf("container_of(ptr_to_b) = %p\n", (void *)owner);
    printf("&t                  = %p\n", (void *)&t);

    /* 简单校验：相等则返回 0 */
    if (owner != &t) {
        fprintf(stderr, "校验失败：地址不一致\n");
        return 1;
    }
    return 0;
}

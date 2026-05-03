/*
 * 调试输出宏实现
 */
#include <stdio.h>
#include <stdlib.h>

/*
 * 调试输出宏定义
 * 根据 DEBUG_LEVEL 的值决定是否启用调试输出
 */
#if DEBUG_LEVEL > 0

/**
 * 调试输出宏 (启用版本)
 *
 * 功能说明：
 * - 输出函数名、行号和自定义消息
 * - 使用 __func__ 获取当前函数名
 * - 使用 __LINE__ 获取当前行号
 * - 支持可变参数，类似 printf
 */
#define DEBUG_PRINT(fmt, ...) \
    do { \
        printf("DEBUG: func=%s, line=%d, " fmt "\n", __func__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#else

/**
 * 调试输出宏 (禁用版本)
 *
 * 当 DEBUG_LEVEL 为 0 时，所有调试输出被禁用
 * 不会产生任何运行时开销
 */
#define DEBUG_PRINT(fmt, ...) do {} while (0)

#endif

/**
 * 测试函数
 *
 * 功能说明：
 * - 验证 DEBUG_PRINT 宏是否正确工作
 * - 输出变量 x 的值和当前行号
 */
void test() {
    int x = 42;
    DEBUG_PRINT("x=%d", x);
}

int main() {
    test();
    return 0;
}

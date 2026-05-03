/*
 * 运算符溢出检测 (使用内联汇编)
 *
 * 本程序演示如何使用 x86 汇编指令检测算术运算的溢出
 *
 * 溢出类型：
 * 1. 加法溢出：无符号数相加结果超出表示范围
 * 2. 减法溢出：小数减大数产生借位
 * 3. 乘法溢出：结果的高位非零
 * 4. 除法溢出：除数为零
 *
 * 检测原理：
 * - CF (Carry Flag): 进位标志，用于无符号运算溢出检测
 * - OF (Overflow Flag): 溢出标志，用于有符号运算溢出检测
 *
 * x86 标志寄存器 (EFLAGS) 相关位：
 * - CF (位0): 进位/借位标志
 * - OF (位11): 溢出标志
 */

#include <stdio.h>
#include <limits.h>

/**
 * 溢出检查结果格式化宏
 */
#define CHECK_OVERFLOW(carry) \
    carry ? "Overflow" : "Not Overflow"

/**
 * 检测无符号整数加法溢出
 *
 * @param a 被加数
 * @param b 加数
 * @return 1 表示溢出，0 表示未溢出
 *
 * 汇编指令说明：
 * - add %2, %1: 执行 a = a + b，结果影响 CF 标志
 * - setc %0: 将 CF 标志的值写入 carry 变量
 *
 * 溢出条件：当结果无法用 32 位表示时，CF 被置 1
 * 例如：UINT_MAX + 1 = 0 (溢出)
 */
int check_add_overflow_asm(unsigned int a, unsigned int b) {
    unsigned char carry;
    __asm__ volatile(
        "add %2, %1\n\t"
        "setc %0"
        : "=r" (carry), "+r" (a)
        : "r" (b)
        : "cc"
    );
    return carry;
}

/**
 * 检测无符号整数减法溢出 (借位)
 *
 * @param a 被减数
 * @param b 减数
 * @return 1 表示借位，0 表示未借位
 *
 * 汇编指令说明：
 * - sub %2, %1: 执行 a = a - b，结果影响 CF 标志
 * - setc %0: 将 CF 标志的值写入 carry 变量
 *
 * 借位条件：当被减数小于减数时，CF 被置 1
 * 例如：0 - 1 = UINT_MAX (借位)
 */
int check_sub_overflow_asm(unsigned int a, unsigned int b) {
    unsigned char carry;
    __asm__ volatile(
        "sub %2, %1\n\t"
        "setc %0"
        : "=r" (carry), "+r" (a)
        : "r" (b)
        : "cc"
    );
    return carry;
}

/**
 * 检测无符号整数乘法溢出
 *
 * @param a 被乘数 (放在 eax 寄存器)
 * @param b 乘数
 * @return 1 表示溢出，0 表示未溢出
 *
 * 汇编指令说明：
 * - mul %3: 执行 eax = eax * b，结果存储在 edx:eax (64位)
 *   - edx: 高32位
 *   - eax: 低32位
 * - seto %0: 将 OF 标志的值写入 overflow 变量
 *
 * 溢出条件：当结果的高32位 (edx) 不为0时，表示溢出
 * 例如：UINT_MAX * 2 需要超过32位来存储
 */
int check_mul_overflow_asm(unsigned int a, unsigned int b) {
    unsigned int high_bits;
    unsigned char overflow;
    __asm__ volatile(
        "mul %3\n\t"
        "seto %0"
        : "=a" (overflow), "=d" (high_bits)
        : "a" (a), "r" (b)
        : "cc"
    );
    return overflow || (high_bits != 0);
}

/**
 * 检测整数除法溢出 (除零错误)
 *
 * @param a 被除数
 * @param b 除数
 * @return 1 表示除零，0 表示正常
 *
 * 汇编指令说明：
 * - test %2, %2: 测试除数 b 是否为0，影响 ZF 标志
 * - sete %0: 将 ZF 标志的值写入 is_div_zero 变量
 *
 * 除零条件：当除数 b 为0时，ZF 被置1
 * @note 除法没有传统的"溢出"概念，只有除零错误
 */
int check_div_overflow_asm(unsigned int a, unsigned int b) {
    unsigned char is_div_zero;
    __asm__ volatile(
        "test %2, %2\n\t"
        "sete %0"
        : "=r" (is_div_zero)
        : "a" (a), "r" (b)
        : "cc"
    );
    return is_div_zero;
}

int main() {
    printf("(UINT_MAX + 1)Add: %s\n", CHECK_OVERFLOW(check_add_overflow_asm(UINT_MAX, 1)));   // 1
    printf("(1, 0)Add: %s\n", CHECK_OVERFLOW(check_add_overflow_asm(1, 0)));  
    printf("(0, 1)Sub: %s\n", CHECK_OVERFLOW(check_sub_overflow_asm(0, 1)));          // 1
    printf("(2, 1)Sub: %s\n", CHECK_OVERFLOW(check_sub_overflow_asm(2, 1)));
    printf("(UINT_MAX, 2)Mul: %s\n", CHECK_OVERFLOW(check_mul_overflow_asm(UINT_MAX, 2)));   // 1
    printf("(1, 2)Mul: %s\n", CHECK_OVERFLOW(check_mul_overflow_asm(1, 2)));
    printf("(10, 0)Div: %s\n", CHECK_OVERFLOW(check_div_overflow_asm(10, 0)));                         // 1
    printf("(2, 1)Div: %s\n", CHECK_OVERFLOW(check_div_overflow_asm(2, 1)));
    return 0;
}
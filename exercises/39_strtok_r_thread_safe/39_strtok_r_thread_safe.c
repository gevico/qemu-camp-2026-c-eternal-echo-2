/*
 * === 线程安全字符串分割器 (strtok_r 实现) ===
 *
 * strtok_r 是 strtok 的可重入版本，解决了线程安全问题
 *
 * 问题：标准库的 strtok 使用静态变量存储状态，多线程环境下不安全
 * 解决：使用额外的 saveptr 参数，让调用者管理状态
 *
 * 函数原型：char* strtok_r(char *str, const char *delim, char **saveptr)
 *
 * 参数说明：
 * - str: 要分割的字符串 (首次调用传入，后续调用传 NULL)
 * - delim: 分隔符集合
 * - saveptr: 保存解析状态的位置指针
 *
 * 使用示例：
 *   char buf[] = "hello,world test";
 *   char *save = NULL;
 *   char *tok = strtok_r(buf, ", ", &save);
 *   while (tok) {
 *       printf("%s\n", tok);
 *       tok = strtok_r(NULL, ", ", &save);
 *   }
 *   输出：
 *   hello
 *   world
 *   test
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * 判断字符是否为分隔符
 *
 * @param c 要判断的字符
 * @param delim 分隔符集合字符串
 * @return 如果是分隔符返回 1，否则返回 0
 */
static int is_delim(char c, const char *delim) {
    while (*delim) {
        if (c == *delim) {
            return 1;
        }
        delim++;
    }
    return 0;
}

/**
 * 线程安全的字符串分割函数
 *
 * @param str 要分割的字符串 (首次调用传入，后续调用传 NULL)
 * @param delim 分隔符集合
 * @param saveptr 保存解析状态的位置指针
 * @return 下一个 token 的指针，没有更多 token 时返回 NULL
 *
 * 工作原理：
 * 1. 首次调用时，str 传入要分割的字符串，保存在 saveptr 中
 * 2. 后续调用时，str 传入 NULL，从 saveptr 保存的位置继续
 * 3. 每次调用返回下一个非分隔符序列，并在原字符串中插入 '\0'
 * 4. 更新 saveptr 指向下一个待处理的位置
 *
 * @note 此函数会修改原字符串 (插入 '\0' 分隔符)
 * @note delim 中的每个字符都是独立的分隔符
 */
char *strtok_r(char *str, const char *delim, char **saveptr) {
    if (!saveptr) {
        return NULL;
    }

    if (str) {
        *saveptr = str;
    }

    if (!*saveptr) {
        return NULL;
    }

    while (is_delim(**saveptr, delim)) {
        if (**saveptr == '\0') {
            *saveptr = NULL;
            return NULL;
        }
        (*saveptr)++;
    }

    char *start = *saveptr;

    while (**saveptr && !is_delim(**saveptr, delim)) {
        (*saveptr)++;
    }

    if (**saveptr) {
        **saveptr = '\0';
        (*saveptr)++;
    } else {
        *saveptr = NULL;
    }

    return start;
}

int main(void) {
    char buf[] = "hello,world test";
    const char *delim = ", ";
    char *save = NULL;

    char *tok = strtok_r(buf, delim, &save);
    while (tok) {
        printf("%s\n", tok);
        tok = strtok_r(NULL, delim, &save);
    }
    return 0;
}

#include <stdio.h>

int main() {
    char source[] = "Hello, World! This is a test string for my_strcpy function.\n";
    char destination[101];
    
    /**
     * 使用双指针逐字符拷贝：
     * - 每次把 *src 复制到 *dst
     * - 复制到 '\0' 时停止，确保目标串正确结束
     */
    char *src = source;
    char *dst = destination;
    while ((*dst++ = *src++) != '\0') {
        // 空循环体：拷贝动作已在条件表达式中完成
    }
    
    printf("拷贝后的字符串: %s", destination);
    
    return 0;
}
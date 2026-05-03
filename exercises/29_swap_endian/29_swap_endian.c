/*
 * 字节序转换工具
 *
 * 字节序 (Endianness) 指多字节数据在内存中的存储顺序
 *
 * 大端序 (Big-Endian):
 * - 高位字节存储在低地址
 * - 网络字节序采用大端序
 * - 示例：0x12345678 在内存中为 [12][34][56][78]
 *
 * 小端序 (Little-Endian):
 * - 低位字节存储在低地址
 * - x86/x64 架构采用小端序
 * - 示例：0x12345678 在内存中为 [78][56][34][12]
 *
 * 应用场景：
 * - 网络编程：需要在主机字节序和网络字节序之间转换
 * - 文件格式：某些文件格式使用特定的字节序
 * - 跨平台数据交换
 */

#include <stdint.h>
#include <stdio.h>

/**
 * 32位整数的字节序转换
 *
 * @param num 要转换的32位整数
 * @return 字节序交换后的值
 *
 * 转换原理：
 * 假设原数为 0xAABBCCDD (大端序表示)
 * 内存布局：[AA][BB][CC][DD]
 * - 字节0 (bits 24-31): AA
 * - 字节1 (bits 16-23): BB
 * - 字节2 (bits 8-15):  CC
 * - 字节3 (bits 0-7):   DD
 *
 * 转换后：0xDDCCBBAA
 * - 第1步：(num >> 24) & 0xFF = AA (移到最低位)
 * - 第2步：(num >> 8) & 0xFF00 = BB00 (移到次低位)
 * - 第3步：(num << 8) & 0xFF0000 = CC0000 (移到次高位)
 * - 第4步：(num << 24) & 0xFF000000 = DD000000 (移到最高位)
 *
 * @note 该函数是自逆的：swap_endian(swap_endian(x)) = x
 */
uint32_t swap_endian(uint32_t num) {
    return ((num >> 24) & 0x000000FF) |  // 字节0移到字节3
           ((num >> 8)  & 0x0000FF00) |  // 字节1移到字节2
           ((num << 8)  & 0x00FF0000) |  // 字节2移到字节1
           ((num << 24) & 0xFF000000);   // 字节3移到字节0
}

/**
 * 主函数：演示字节序转换
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return 程序退出状态码
 *
 * 测试用例：0x78563412 (十六进制数字表示各字节位置)
 * - 字节0: 0x78
 * - 字节1: 0x56
 * - 字节2: 0x34
 * - 字节3: 0x12
 *
 * 转换后应为：0x12345678
 */
int main(int argc, char* argv[]) {
    uint32_t num = 0x78563412;
    uint32_t swapped = swap_endian(num);
    printf("0x%08x -> 0x%08x\n", num, swapped);
    return 0;
}
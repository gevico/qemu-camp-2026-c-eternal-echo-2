/*
 * ELF 文件信息查看命令 (myfile)
 *
 * 这是 mybash 的一个内置命令，用于显示 ELF 文件的基本信息
 *
 * ELF (Executable and Linkable Format) 文件格式：
 * - ELF Header: 包含文件类型、入口地址、程序头表位置等元数据
 * - Program Header Table: 描述程序加载到内存时的段信息
 * - Section Header Table: 描述代码段、数据段等各个节的信息
 *
 * 本命令功能：
 * 1. 验证文件是否为有效的 ELF 文件
 * 2. 显示 ELF 魔数
 * 3. 显示文件位数 (32/64 位)
 * 4. 显示字节序 (大端/小端)
 * 5. 显示 ELF 文件类型
 */

#include "myfile.h"

/**
 * 打印 ELF 文件类型
 *
 * @param e_type ELF 头中的类型字段 (16位无符号整数)
 *
 * @note 该函数会额外检查 OS 特定和处理器特定的类型范围
 */
void print_elf_type(uint16_t e_type) {
    const char *type_str;
    switch (e_type) {
    case ET_NONE:
      type_str = "Unknown (ET_NONE)";
      break;
    case ET_REL:
      type_str = "Relocatable (ET_REL)";
      break;
    case ET_EXEC:
      type_str = "Executable (ET_EXEC)";
      break;
    case ET_DYN:
      type_str = "Shared Object/PIE (ET_DYN)";
      break;
    case ET_CORE:
      type_str = "Core Dump (ET_CORE)";
      break;
    default:
      if (e_type >= ET_LOOS && e_type <= ET_HIOS)
        type_str = "OS-Specific";
      else if (e_type >= ET_LOPROC && e_type <= ET_HIPROC)
        type_str = "Processor-Specific";
      else
        type_str = "Invalid";
    }
    printf("ELF Type: %s (0x%x)\n", type_str, e_type);
}

/**
 * myfile 命令的主函数
 *
 * 解析并显示指定 ELF 文件的信息
 *
 * @param filename 要分析的 ELF 文件路径
 * @return 成功返回 0，失败返回 1
 *
 * ELF 解析步骤：
 * 1. 打开目标文件
 * 2. 读取 ELF 头 (Elf64_Ehdr 结构体，64 字节)
 * 3. 验证 ELF 魔数 (0x7f 'E' 'L' 'F')
 * 4. 提取并显示文件属性
 */
int __cmd_myfile(const char* filename) {
    char filepath[256];
    int fd;
    Elf64_Ehdr ehdr;  // 64位 ELF 文件头结构体

    // 复制文件路径到本地缓冲区
    strcpy(filepath, filename);
    fflush(stdout);
    printf("filepath: %s\n", filepath);

    // 以只读方式打开文件
    fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    // 读取 ELF 文件头
    // Elf64_Ehdr 结构体包含所有 ELF 元数据
    if (read(fd, &ehdr, sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr)) {
        perror("Failed to read ELF header");
        close(fd);
        return 1;
    }

    // 验证 ELF 魔数
    // e_ident[0-3] 应该是 0x7f 'E' 'L' 'F'
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Not a valid ELF file\n");
        close(fd);
        return 1;
    }

    // 输出 ELF 文件信息
    printf("ELF Magic: %.4s\n", ehdr.e_ident);  // 魔数
    printf("ELF Class: %s\n", ehdr.e_ident[EI_CLASS] == ELFCLASS64 ? "64-bit" : "32-bit");  // 位数
    printf("ELF Data: %s\n", ehdr.e_ident[EI_DATA] == ELFDATA2LSB ? "Little-endian" : "Big-endian");  // 字节序

    print_elf_type(ehdr.e_type);  // 文件类型
    close(fd);
    return 0;
}
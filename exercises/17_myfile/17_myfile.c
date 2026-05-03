/*
 * ELF 文件信息查看工具
 *
 * ELF (Executable and Linkable Format) 是 Linux 系统中常用的可执行文件格式
 * 本程序演示如何解析 ELF 文件头，提取文件的类型、位数和字节序信息
 *
 * ELF 文件结构：
 * 1. ELF Header (文件头) - 包含文件的基本信息
 * 2. Program Header Table (程序头表) - 描述程序的段信息
 * 3. Sections (段) - 实际的代码和数据
 * 4. Section Header Table (段头表) - 描述各段的信息
 */

#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * 打印 ELF 文件类型
 *
 * 根据 ELF 头中的 e_type 字段，输出文件的可执行类型
 *
 * @param e_type ELF 头中的类型字段 (16位)
 *
 * ELF 文件类型说明：
 * - ET_NONE: 未知类型
 * - ET_REL: 可重定位文件 (.o 目标文件)
 * - ET_EXEC: 可执行文件
 * - ET_DYN: 共享目标文件 (动态链接库)
 * - ET_CORE: 核心转储文件
 */
void print_elf_type(uint16_t e_type) {
  const char *type_str;
  switch (e_type) {
    case ET_NONE:
      type_str = "Unknown";
      break;
    case ET_REL:
      type_str = "Relocatable";
      break;
    case ET_EXEC:
      type_str = "Executable";
      break;
    case ET_DYN:
      type_str = "Shared Object/PIE";
      break;
    case ET_CORE:
      type_str = "Core";
      break;
    default:
      type_str = "Unknown";
      break;
  }
  printf("ELF Type: %s (0x%x)\n", type_str, e_type);
}

/**
 * 主函数：解析并显示 ELF 文件信息
 *
 * 程序流程：
 * 1. 打开目标文件 (17_myfile.o 和 17_myfile)
 * 2. 读取 ELF 文件头 (ELF Header)
 * 3. 验证 ELF 魔数 (Magic Number)
 * 4. 提取并显示文件类型、位数、字节序等信息
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return 程序退出状态码
 */
int main(int argc, char *argv[]) {
  // 要检查的文件路径数组
  // 17_myfile.o 是编译后的目标文件 (可重定位文件)
  // 17_myfile 是链接后的可执行文件
  char filepath[2][256] = {
    "./17_myfile.o",
    "./17_myfile",
  };

  int fd;
  Elf64_Ehdr ehdr;  // 64位 ELF 文件头结构体

  for (int i = 0; i < 2; i++) {
    printf("\nChecking file: %s\n", filepath[i]);

    // 以只读方式打开文件
    fd = open(filepath[i], O_RDONLY);
    if (fd < 0) {
      perror("Failed to open file");
      continue;
    }

    // 读取 ELF 文件头 (共 64 字节)
    if (read(fd, &ehdr, sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr)) {
      perror("Failed to read ELF header");
      close(fd);
      continue;
    }

    // 验证 ELF 魔数
    // ELF 文件的前 4 个字节必须是 0x7f 'E' 'L' 'F'
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
      printf("Not a valid ELF file\n");
      close(fd);
      continue;
    }

    // 输出 ELF 文件信息
    printf("ELF Magic: %.4s\n", ehdr.e_ident);  // 输出魔数
    printf("ELF Class: %s\n", ehdr.e_ident[EI_CLASS] == ELFCLASS64 ? "64-bit" : "32-bit");  // 文件位数
    printf("ELF Data: %s\n", ehdr.e_ident[EI_DATA] == ELFDATA2LSB ? "Little-endian" : "Big-endian");  // 字节序

    // 输出 ELF 文件类型
    print_elf_type(ehdr.e_type);

    close(fd);
  }

  return 0;
}

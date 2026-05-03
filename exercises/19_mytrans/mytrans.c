/*
 * 文本翻译工具 (英汉词典)
 *
 * 本程序实现一个简单的英汉翻译系统，使用哈希表存储单词和译文
 *
 * 功能：
 * - 从文本文件加载词典 (单词 - 译文对)
 * - 处理用户输入的英文单词，输出对应的中文翻译
 *
 * 词典格式：
 * word1
 * translation1
 * (空行分隔词条)
 * word2
 * translation2
 * ...
 */

// mytrans.c
#include "myhash.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * 去除字符串两端的空白字符
 *
 * @param str 要处理的字符串 (原地修改)
 *
 * 处理步骤：
 * 1. 去除前导空白字符
 * 2. 去除尾部空白字符
 * 3. 将字符串移到缓冲区起始位置
 */
void trim(char *str) {
    if (!str)
        return;

    // 去除前导空白
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    // 去除尾部空白
    char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';

    // 移动字符串
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/**
 * 从文件加载词典到哈希表
 *
 * @param filename 词典文件路径
 * @param table 哈希表指针
 * @param dict_count 输出参数，返回加载的词条数量
 * @return 成功返回 0，失败返回 -1
 *
 * 文件格式说明：
 * - 每个词条由单词行和多行译文组成
 * - 词条之间用空行分隔
 * - 单词行通常是词条的第一行
 *
 * 解析算法：
 * 1. 逐行读取文件
 * 2. 遇到空行表示当前词条结束，存入哈希表
 * 3. 非空行根据状态判断是单词还是译文
 */
int load_dictionary(const char *filename, HashTable *table,
                    uint64_t *dict_count) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("无法打开词典文件");
    return -1;
  }

  char line[1024];
  char current_word[100] = {0};
  char current_translation[1024] = {0};
  int in_entry = 0;
  *dict_count = 0;

  while (fgets(line, sizeof(line), file)) {
    // 移除换行符
    line[strcspn(line, "\n")] = '\0';
    trim(line);

    // 以 # 开头表示单词行
    if (line[0] == '#') {
      // 如果之前有词条，先保存
      if (in_entry && strlen(current_word) > 0) {
        hash_table_insert(table, current_word, current_translation);
        (*dict_count)++;
      }
      // 新词条开始，去掉 # 前缀
      strncpy(current_word, line + 1, sizeof(current_word) - 1);
      current_word[sizeof(current_word) - 1] = '\0';
      trim(current_word);
      strncpy(current_translation, "", sizeof(current_translation));
      in_entry = 1;
    } else if (strncmp(line, "Trans:", 6) == 0) {
      // 翻译内容行，去掉 "Trans:" 前缀
      char *trans = line + 6;
      trim(trans);
      if (strlen(current_translation) > 0) {
        strncat(current_translation, "@", sizeof(current_translation) - strlen(current_translation) - 1);
      }
      strncat(current_translation, trans, sizeof(current_translation) - strlen(current_translation) - 1);
    }
  }

  // 处理最后一个词条
  if (in_entry && strlen(current_word) > 0) {
    hash_table_insert(table, current_word, current_translation);
    (*dict_count)++;
  }

  fclose(file);
  return 0;
}

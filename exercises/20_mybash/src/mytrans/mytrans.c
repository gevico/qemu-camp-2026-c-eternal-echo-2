// mytrans.c
#include "myhash.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  *dict_count = 0;

  while (fgets(line, sizeof(line), file)) {
    // 移除换行符
    line[strcspn(line, "\n")] = '\0';
    trim(line);

    // 跳过空行
    if (strlen(line) == 0) {
      continue;
    }

    // 处理单词行（以 # 开头）
    if (line[0] == '#') {
      // 如果之前有词条，先保存
      if (strlen(current_word) > 0) {
        hash_table_insert(table, current_word, current_translation);
        (*dict_count)++;
      }
      // 提取单词（去掉 # 前缀）
      strncpy(current_word, line + 1, sizeof(current_word) - 1);
      current_word[sizeof(current_word) - 1] = '\0';
      trim(current_word);
      current_translation[0] = '\0';
    }
    // 处理翻译行（以 Trans: 开头）
    else if (strncmp(line, "Trans:", 6) == 0) {
      char *trans = line + 6;  // 跳过 "Trans:" 前缀
      trim(trans);
      if (strlen(current_translation) > 0) {
        strncat(current_translation, "@", sizeof(current_translation) - strlen(current_translation) - 1);
      }
      strncat(current_translation, trans, sizeof(current_translation) - strlen(current_translation) - 1);
    }
  }

  // 处理最后一个词条
  if (strlen(current_word) > 0) {
    hash_table_insert(table, current_word, current_translation);
    (*dict_count)++;
  }

  fclose(file);
  return 0;
}

void to_lowercase(char *str) {
  for (; *str; ++str)
    *str = tolower((unsigned char)*str);
}

int __cmd_mytrans(const char* filename) {
  HashTable *table = create_hash_table();
  if (!table) {
    fprintf(stderr, "无法创建哈希表\n");
    return 1;
  }

  printf("=== 哈希表版英语翻译器（支持百万级数据）===\n");
  uint64_t dict_count = 0;
  if (load_dictionary("./src/mytrans/dict.txt", table, &dict_count) != 0) {
    fprintf(stderr, "加载词典失败，请确保 dict.txt 存在。\n");
    free_hash_table(table);
    return 1;
  }
  printf("词典加载完成，共计%ld词条。\n", dict_count);

  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "无法打开文件 dict.txt。\n");
    free_hash_table(table);
    return 1;
  }

  char line[256];
  while (fgets(line, sizeof(line), file) != NULL) {
    line[strcspn(line, "\n")] = '\0';

    if (strlen(line) == 0) {
        continue;
    }

    // 使用 strtok 按空格分割单词
    char *word = strtok(line, " ");
    while (word != NULL) {
      const char *translation = hash_table_lookup(table, word);
      printf("原文: %s\t", word);
      if (translation) {
          printf("翻译: %s\n", translation);
      } else {
          printf("未找到该单词的翻译。\n");
      }

      word = strtok(NULL, " ");
    }
  }

  free_hash_table(table);
  return 0;
}
// main.c
#include "myhash.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void to_lowercase(char *str) {
  for (; *str; ++str)
    *str = tolower((unsigned char)*str);
}

int main() {
  HashTable *table = create_hash_table();
  if (!table) {
    fprintf(stderr, "无法创建哈希表\n");
    return 1;
  }

  printf("=== 哈希表版英语翻译器（支持百万级数据）===\n");
  uint64_t dict_count = 0;
  if (load_dictionary("dict.txt", table, &dict_count) != 0) {
    fprintf(stderr, "加载词典失败，请确保 dict.txt 存在。\n");
    free_hash_table(table);
    return 1;
  }
  printf("词典加载完成，共计%ld词条。\n", dict_count);

  FILE* file = fopen("text.txt", "r");
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
      // 清理单词中的标点符号
      char clean_word[256] = {0};
      int j = 0;
      for (int i = 0; word[i] && j < sizeof(clean_word) - 1; i++) {
        if (isalpha((unsigned char)word[i]) || word[i] == '\'') {
          clean_word[j++] = tolower((unsigned char)word[i]);
        }
      }
      clean_word[j] = '\0';

      if (strlen(clean_word) > 0) {
        const char *translation = hash_table_lookup(table, clean_word);
        if (translation) {
          printf("原文: %s\t翻译: %s\n", clean_word, translation);
        } else {
          printf("原文: %s\t未找到该单词的翻译。\n", clean_word);
        }
      }
      word = strtok(NULL, " ");
    }
  }

  free_hash_table(table);
  return 0;
}

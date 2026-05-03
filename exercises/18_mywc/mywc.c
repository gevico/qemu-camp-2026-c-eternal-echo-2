/*
 * 单词频率统计工具
 *
 * 本程序统计文本文件中每个单词出现的次数，使用哈希表实现 O(1) 平均查找时间
 *
 * 数据结构：链式哈希表 (Chained Hash Table)
 * - 哈希函数：djb2 算法 (hash * 33 + c)
 * - 冲突解决：链地址法 (每个桶是一个链表)
 *
 * 单词定义：由字母和单引号组成的连续字符序列
 * 所有单词统一转换为小写后统计
 */

#include "mywc.h"

/**
 * 创建哈希表
 *
 * 分配并初始化一个包含 HASH_SIZE 个桶的哈希表
 * 所有桶初始化为 NULL (空链表)
 *
 * @return 指向哈希表的指针，失败返回 NULL
 */
WordCount **create_hash_table() {
  WordCount **hash_table = calloc(HASH_SIZE, sizeof(WordCount *));
  return hash_table;
}

/**
 * djb2 哈希函数
 *
 * 这是一个广泛使用的字符串哈希算法，由 Daniel J. Bernstein 设计
 * 算法：hash = hash * 33 + c，其中 33 = 2^5 + 1，可以用移位优化为 hash << 5 + hash
 *
 * @param word 要计算哈希值的字符串
 * @return 哈希值 (0 到 HASH_SIZE-1)
 *
 * 时间复杂度：O(n)，n 为字符串长度
 */
unsigned int hash(const char *word) {
  unsigned long hash = 5381;
  int c;
  while ((c = *word++))
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  return hash % HASH_SIZE;
}

/**
 * 检查字符是否可以作为单词的一部分
 *
 * 单词有效字符：字母 (a-z, A-Z) 和单引号 (')
 * 这允许处理如 "don't"、"it's" 等缩写形式
 *
 * @param c 要检查的字符
 * @return 如果是有效字符返回 true，否则返回 false
 */
bool is_valid_word_char(char c) { return isalpha(c) || c == '\''; }

/**
 * 将字符转换为小写
 *
 * @param c 要转换的字符
 * @return 转换后的小写字符
 */
char to_lower(char c) { return tolower(c); }

/**
 * 向哈希表中添加或更新单词计数
 *
 * 操作步骤：
 * 1. 计算单词的哈希值，定位到对应的桶
 * 2. 在该桶的链表中查找单词是否已存在
 * 3. 如果存在，增加计数；如果不存在，创建新节点并插入链表头部
 *
 * @param hash_table 哈希表指针
 * @param word 要添加的单词字符串
 *
 * 时间复杂度：平均 O(1)，最坏 O(n) 当所有单词都在同一个桶时
 */
void add_word(WordCount **hash_table, const char *word) {
    unsigned int index = hash(word);
    WordCount *current = hash_table[index];

    // 查找单词是否已存在
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            current->count++;
            return;
        }
        current = current->next;
    }

    // 创建新节点
    WordCount *new_node = malloc(sizeof(WordCount));
    strncpy(new_node->word, word, MAX_WORD_LEN - 1);
    new_node->word[MAX_WORD_LEN - 1] = '\0';
    new_node->count = 1;
    new_node->next = hash_table[index];
    hash_table[index] = new_node;
}

/**
 * 打印单词统计结果
 *
 * 遍历哈希表的所有桶和链表，输出每个单词及其出现次数
 * 输出格式：单词左对齐占20字符，后跟出现次数
 *
 * @param hash_table 哈希表指针
 */
void print_word_counts(WordCount **hash_table) {
  printf("Word Count Statistics:\n");
  printf("======================\n");

  for (int i = 0; i < HASH_SIZE; i++) {
    WordCount *current = hash_table[i];
    while (current != NULL) {
      printf("%-20s %d\n", current->word, current->count);
      current = current->next;
    }
  }
}

/**
 * 释放哈希表占用的所有内存
 *
 * 遍历所有桶，释放每个链表中的所有节点，最后释放哈希表本身
 *
 * @param hash_table 哈希表指针
 */
void free_hash_table(WordCount **hash_table) {
    for (int i = 0; i < HASH_SIZE; i++) {
        WordCount *current = hash_table[i];
        while (current != NULL) {
            WordCount *temp = current;
            current = current->next;
            free(temp);
        }
        hash_table[i] = NULL;
    }
    free(hash_table);
}

/**
 * 处理文件并统计单词频率
 *
 * 操作步骤：
 * 1. 打开文件并创建哈希表
 * 2. 逐字符读取文件，识别单词边界
 * 3. 将识别出的单词转换为小写并存入哈希表
 * 4. 文件读取完毕后，打印统计结果并释放内存
 *
 * @param filename 要处理的文件路径
 *
 * 单词识别规则：
 * - 有效字符：字母和单引号
 * - 单词边界：遇到非有效字符时，当前单词结束
 * - 最大单词长度：MAX_WORD_LEN - 1
 */
void process_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  WordCount **hash_table = create_hash_table();
  char word[MAX_WORD_LEN];
  int word_pos = 0;
  int c;

  while ((c = fgetc(file)) != EOF) {
    if (is_valid_word_char(c)) {
      if (word_pos < MAX_WORD_LEN - 1) {
        word[word_pos++] = to_lower(c);
      }
    } else {
      if (word_pos > 0) {
        word[word_pos] = '\0';
        add_word(hash_table, word);
        word_pos = 0;
      }
    }
  }

  // 处理文件末尾的最后一个单词
  if (word_pos > 0) {
    word[word_pos] = '\0';
    add_word(hash_table, word);
  }

  fclose(file);
  print_word_counts(hash_table);
  free_hash_table(hash_table);
}

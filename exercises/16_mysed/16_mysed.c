#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024

int parse_replace_command(const char* cmd, char** old_str, char** new_str) {
    if (cmd[0] != 's' || cmd[1] != '/') {
        return -1;
    }

    /**
     * 解析规则 s/old/new/：
     * - 第一个 '/' 后到第二个 '/' 为 old
     * - 第二个 '/' 后到第三个 '/' 为 new
     */
    const char *p1 = cmd + 2;
    const char *p2 = strchr(p1, '/');
    if (p2 == NULL) {
        return -1;
    }
    const char *p3 = strchr(p2 + 1, '/');
    if (p3 == NULL) {
        return -1;
    }

    size_t old_len = (size_t)(p2 - p1);
    size_t new_len = (size_t)(p3 - (p2 + 1));

    *old_str = (char *)malloc(old_len + 1);
    *new_str = (char *)malloc(new_len + 1);
    if (*old_str == NULL || *new_str == NULL) {
        free(*old_str);
        free(*new_str);
        *old_str = NULL;
        *new_str = NULL;
        return -1;
    }

    memcpy(*old_str, p1, old_len);
    (*old_str)[old_len] = '\0';
    memcpy(*new_str, p2 + 1, new_len);
    (*new_str)[new_len] = '\0';

    return 0;
}

void replace_first_occurrence(char* str, const char* old, const char* new) {
    /**
     * 仅替换第一次出现的 old：
     * - 找到 old 在 str 中的位置
     * - 拼接前缀 + new + 后缀到临时缓冲，再拷回 str
     */
    char *pos = strstr(str, old);
    if (pos == NULL) {
        return;
    }

    char buffer[MAX_LINE_LENGTH];
    size_t prefix_len = (size_t)(pos - str);
    size_t old_len = strlen(old);
    size_t new_len = strlen(new);

    memcpy(buffer, str, prefix_len);
    memcpy(buffer + prefix_len, new, new_len);
    strcpy(buffer + prefix_len + new_len, pos + old_len);
    strcpy(str, buffer);
}

int main(int argc, char* argv[]) {
    const char* replcae_rules = "s/unix/linux/";

    char line[MAX_LINE_LENGTH] = {"unix is opensource. unix is free os."};

    char* old_str = NULL;
    char* new_str = NULL;
    
    if (parse_replace_command(replcae_rules, &old_str, &new_str) != 0) {
        fprintf(stderr, "Invalid replace command format. Use 's/old/new/'\n");
        return 1;
    }

    
    replace_first_occurrence(line, old_str, new_str);
    fputs(line, stdout);

    free(old_str);
    free(new_str);
    return 0;
}

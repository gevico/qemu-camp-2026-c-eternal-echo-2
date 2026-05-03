#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
 * URL参数解析器
 * 输入：包含http/https超链接的字符串
 * 输出：解析出所有的key-value键值对，每行显示一个
 */

int parse_url(const char* url) {
    int err = 0;

    /**
     * 解析步骤：
     * 1) 找到 '?' 后的查询串
     * 2) 按 '&' 拆分参数项
     * 3) 每项按 '=' 拆分为 key/value 并输出
     */
    const char *query = strchr(url, '?');
    if (query == NULL || *(query + 1) == '\0') {
        err = EINVAL;
        goto exit;
    }
    query++;  // 跳过 '?'

    char *buf = (char *)malloc(strlen(query) + 1);
    if (buf == NULL) {
        err = ENOMEM;
        goto exit;
    }
    strcpy(buf, query);

    char *item = strtok(buf, "&");
    while (item != NULL) {
        char *eq = strchr(item, '=');
        if (eq != NULL) {
            *eq = '\0';
            printf("key = %s, value = %s\n", item, eq + 1);
        }
        item = strtok(NULL, "&");
    }

    free(buf);

exit:
    return err;
}

int main() {
    const char* test_url = "https://cn.bing.com/search?name=John&age=30&city=New+York";

    printf("Parsing URL: %s\n", test_url);
    printf("Parameters:\n");

    parse_url(test_url);

    return 0;
}
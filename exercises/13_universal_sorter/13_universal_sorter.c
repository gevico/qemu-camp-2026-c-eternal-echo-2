#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*CompareFunc)(const void *, const void *);

int compareInt(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int compareFloat(const void *a, const void *b) {
    float diff = (*(float*)a - *(float*)b);
    return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
}

int compareString(const void *a, const void *b) {
    return strcmp(*(char**)a, *(char**)b);
}

void sort(void *array, size_t n, size_t size, CompareFunc compare) {
    qsort(array, n, size, compare);
}

void processFile(const char *filename) {
    FILE *fin = fopen(filename, "r");
    if (!fin) {
        printf("错误: 无法打开文件 %s\n", filename);
        return;
    }

    int choice, n;
    if (fscanf(fin, "%d", &choice) != 1 || fscanf(fin, "%d", &n) != 1) {
        printf("错误: 文件 %s 格式不正确\n", filename);
        fclose(fin);
        return;
    }

    if (n > 20) n = 20;  // 最多支持20个元素

    printf("=== 处理数据来自: %s ===\n", filename);

    switch (choice) {
        case 1: {
            /**
             * choice=1：读取并排序整数数组。
             * 输出格式保持为“升序结果 + 空格分隔”。
             */
            int arr[20];
            for (int i = 0; i < n; i++) {
                if (fscanf(fin, "%d", &arr[i]) != 1) {
                    printf("错误: 整数数据读取失败\n");
                    fclose(fin);
                    return;
                }
            }
            sort(arr, (size_t)n, sizeof(int), compareInt);
            printf("排序结果: ");
            for (int i = 0; i < n; i++) {
                printf("%d%s", arr[i], (i == n - 1) ? "\n" : " ");
            }
            break;
        }
        case 2: {
            /**
             * choice=2：读取并排序浮点数组。
             * 使用 %.2f 输出，便于观察排序结果。
             */
            float arr[20];
            for (int i = 0; i < n; i++) {
                if (fscanf(fin, "%f", &arr[i]) != 1) {
                    printf("错误: 浮点数据读取失败\n");
                    fclose(fin);
                    return;
                }
            }
            sort(arr, (size_t)n, sizeof(float), compareFloat);
            printf("排序结果: ");
            for (int i = 0; i < n; i++) {
                printf("%.2f%s", arr[i], (i == n - 1) ? "\n" : " ");
            }
            break;
        }
        case 3: {
            /**
             * choice=3：读取并排序字符串数组。
             * 每个字符串单独申请存储，排序后统一释放。
             */
            char *arr[20];
            char buf[256];
            for (int i = 0; i < n; i++) {
                if (fscanf(fin, "%255s", buf) != 1) {
                    printf("错误: 字符串数据读取失败\n");
                    for (int j = 0; j < i; j++) {
                        free(arr[j]);
                    }
                    fclose(fin);
                    return;
                }
                arr[i] = (char *)malloc(strlen(buf) + 1);
                if (!arr[i]) {
                    printf("错误: 内存分配失败\n");
                    for (int j = 0; j < i; j++) {
                        free(arr[j]);
                    }
                    fclose(fin);
                    return;
                }
                strcpy(arr[i], buf);
            }

            sort(arr, (size_t)n, sizeof(char *), compareString);
            printf("排序结果: ");
            for (int i = 0; i < n; i++) {
                printf("%s%s", arr[i], (i == n - 1) ? "\n" : " ");
                free(arr[i]);
            }
            break;
        }
        default:
            printf("错误: 不支持的数据类型选择 %d\n", choice);
            break;
    }

    fclose(fin);
}

int main() {
    processFile("int_sort.txt");
    processFile("float_sort.txt");

    return 0;
}
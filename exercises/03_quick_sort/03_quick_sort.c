#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define NAME_LEN     50

typedef struct {
    char name[NAME_LEN];
    int score;
} Student;

Student students[MAX_STUDENTS];

/**
 * @brief 对全局 students 数组区间执行快速排序（按成绩从高到低）。
 *
 * 使用双指针分区：选取区间中点分数作为基准值 pivot，
 * 左指针寻找比 pivot 小的元素，右指针寻找比 pivot 大的元素，
 * 当两者都找到后交换，以保证左侧分数不低于 pivot、右侧分数不高于 pivot。
 * 分区完成后递归处理左右子区间。
 *
 * @param left 待排序区间左边界（包含）。
 * @param right 待排序区间右边界（包含）。
 *
 * @note 当 left >= right 时，区间长度不超过 1，直接返回。
 */
void quick_sort(int left, int right) {
    if (left >= right) {
        return;
    }

    int i = left;
    int j = right;
    int pivot = students[left + (right - left) / 2].score;

    // 分区：把高分放到左侧，低分放到右侧
    while (i <= j) {
        while (students[i].score > pivot) {
            i++;
        }
        while (students[j].score < pivot) {
            j--;
        }

        if (i <= j) {
            Student tmp = students[i];
            students[i] = students[j];
            students[j] = tmp;
            i++;
            j--;
        }
    }

    if (left < j) {
        quick_sort(left, j);
    }
    if (i < right) {
        quick_sort(i, right);
    }
}

int main(void) {
    FILE *file = fopen("03_students.txt", "r");
    if (!file) {
        printf("错误：无法打开文件 03_students.txt\n");
        return 1;
    }

    int n;
    fscanf(file, "%d", &n);

    if (n <= 0 || n > MAX_STUDENTS) {
        printf("学生人数无效：%d（应为 1-%d）\n", n, MAX_STUDENTS);
        fclose(file);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        fscanf(file, "%s %d", students[i].name, &students[i].score);
    }
    fclose(file);

    quick_sort(0, n - 1);

    // 输出结果
    printf("\n快速排序后按成绩从高到低排序的学生名单：\n");
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", students[i].name, students[i].score);
    }

    return 0;
}
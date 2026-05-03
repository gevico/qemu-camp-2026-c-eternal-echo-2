#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define NAME_LEN 50

typedef struct {
    char name[NAME_LEN];
    int score;
} Student;

Student students[MAX_STUDENTS];
Student temp[MAX_STUDENTS];  // 归并缓冲区：先写合并结果，避免覆盖 students 中未比较元素

/**
 * @brief 对全局 students 数组的指定区间进行归并排序（按成绩从高到低）。
 *
 * 该函数使用分治法递归排序 students[left..right]，并借助全局临时数组 temp
 * 完成有序子区间的合并。排序结果会原地写回 students 数组。
 * 基于**分治**。不断将数组对半分，直到每个子数组只有一个元素。然后将这些有序的子数组两两合并，最终得到一个完全有序的数组。
 *
 * @param left 待排序区间左边界（包含）。
 * @param right 待排序区间右边界（包含）。
 *
 * @note 当 left >= right 时，区间长度不超过 1，递归终止。
 */
void merge_sort(int left, int right) {
    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;
    merge_sort(left, mid);
    merge_sort(mid + 1, right);

    int i = left;
    int j = mid + 1;
    int k = left;

    // 合并两个有序区间 [left..mid] 和 [mid+1..right]（按分数降序）
    while (i <= mid && j <= right) {
        if (students[i].score >= students[j].score) {
            temp[k++] = students[i++];
        } else {
            temp[k++] = students[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = students[i++];
    }
    while (j <= right) {
        temp[k++] = students[j++];
    }

    for (int idx = left; idx <= right; idx++) {
        students[idx] = temp[idx];
    }
}

int main(void) {
    FILE *file = fopen("02_students.txt", "r");
    if (!file) {
        printf("错误：无法打开文件 02_students.txt\n");
        return 1;
    }

    int n;
    fscanf(file, "%d", &n);

    if (n <= 0 || n > MAX_STUDENTS) {
        printf("学生人数无效：%d\n", n);
        fclose(file);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        fscanf(file, "%s %d", students[i].name, &students[i].score);
    }
    fclose(file);

    merge_sort(0, n - 1);

    printf("\n归并排序后按成绩从高到低排序的学生名单：\n");
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", students[i].name, students[i].score);
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[20];
    int score;
} Student;

/**
 * @brief 使用插入排序按成绩从高到低排序学生数组。
 *
 * 该函数会原地修改输入数组：遍历 students[0..n-1]，
 * 将每个学生插入到前面已排序区间中的正确位置，
 * 最终得到按 score 降序排列的结果。
 *
 * @param students 学生数组首地址。每个元素包含 name 和 score。
 * @param n 数组中有效学生数量。仅排序 students[0..n-1]。
 *
 * @note 当 n <= 1 时，数组视为已排序，函数不会进行实际移动。
 */
void insertion_sort(Student students[], int n) {
    // 插入排序：维护 [0..i-1] 为已排序区间（按成绩从高到低）
    for (int i = 1; i < n; i++) {
        Student key = students[i];  // 当前待插入元素
        int j = i - 1;

        // 把比分数更低的元素整体后移，为 key 腾出插入位置
        while (j >= 0 && students[j].score < key.score) {
            students[j + 1] = students[j];
            j--;
        }

        // 将 key 放到正确位置
        students[j + 1] = key;
    }
}

int main(void) {
    FILE *file;
    Student students[50];
    int n = 0;
    
    // 打开文件（从命令行参数获取文件名）
    file = fopen("01_students.txt", "r");
    if (!file) {
        printf("错误：无法打开文件 01_students.txt\n");
        return 1;
    }
    
    // 从文件读取学生信息
    while (n < 50 && fscanf(file, "%s %d", students[n].name, &students[n].score) == 2) {
        n++;
    }
    fclose(file);
    
    if (n == 0) {
        printf("文件中没有学生信息\n");
        return 1;
    }
    
    insertion_sort(students, n);
    
    printf("\n按成绩从高到低排序后的学生信息:\n");
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", students[i].name, students[i].score);
    }
    
    return 0;
}


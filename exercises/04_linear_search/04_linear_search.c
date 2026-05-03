#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 50
#define NAME_LEN     50

typedef struct {
    char name[NAME_LEN];
    int score;
} Student;

Student students[MAX_STUDENTS];
int n;
/**
 * @brief 在线性表 students 中按姓名顺序查找目标学生。
 *
 * 从下标 0 开始逐个比较 students[i].name 与 target_name，
 * 若匹配则返回对应下标；若遍历结束仍未找到则返回 -1。
 *
 * @param target_name 需要查找的学生姓名（以 '\0' 结尾）。
 * @return int 找到返回下标，未找到返回 -1。
 */
int linear_search(const char *target_name) {
    for (int i = 0; i < n; i++) {
        if (strcmp(students[i].name, target_name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    FILE *file = fopen("04_students.txt", "r");
    if (!file) {
        printf("错误：无法打开文件 04_students.txt\n");
        return 1;
    }

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

    char query_name[NAME_LEN] = "David";

    int index = linear_search(query_name);

    printf("\n线性查找出的学生信息：\n");
    if (index != -1) {
        printf("姓名：%s，成绩：%d\n", students[index].name, students[index].score);
    } else {
        printf("未找到该学生\n");
    }

    return 0;
}
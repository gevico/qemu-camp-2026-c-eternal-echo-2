#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char id[20];
    char name[50];
    int age;
} Student;

int main() {
    FILE *file = fopen("students.txt", "r");
    if (file == NULL) {
        printf("无法打开文件\n");
        return 1;
    }
    
    Student *students[3];
    
    for (int i = 0; i < 3; i++) 
    {
        /**
         * 每次循环动态申请一个 Student 节点，
         * 然后从文件读取“学号 姓名 年龄”三个字段填充结构体。
         */
        students[i] = (Student *)malloc(sizeof(Student));
        if (students[i] == NULL) {
            printf("内存分配失败\n");
            fclose(file);
            for (int j = 0; j < i; j++) {
                free(students[j]);
            }
            return 1;
        }

        if (fscanf(file, "%19s %49s %d",
                   students[i]->id,
                   students[i]->name,
                   &students[i]->age) != 3) {
            printf("文件数据格式错误\n");
            fclose(file);
            for (int j = 0; j <= i; j++) {
                free(students[j]);
            }
            return 1;
        }
    }
    fclose(file);
    
    for (int i = 0; i < 3; i++) 
    {
        printf("学号：%s, 姓名：%s, 年龄：%d\n", 
               students[i]->id, 
               students[i]->name, 
               students[i]->age);
    }
    
    for (int i = 0; i < 3; i++) 
    {
        free(students[i]);
    }
    
    return 0;
}
#include <stdio.h>
#include <stdbool.h>

#define MAX_PEOPLE 50

typedef struct {
    int id;
} People;

typedef struct {
    People data[MAX_PEOPLE];
    int head;
    int tail;
    int count;
} Queue;

int main() {
    Queue q;
    int total_people=50;
    int report_interval=5;

    /**
     * 使用环形队列模拟约瑟夫环：
     * - 初始化 1..50 入队
     * - 每轮把前 4 个人出队后再入队（相当于报数 1~4）
     * - 第 5 个人出队并淘汰
     * - 重复直到只剩 1 人
     */
    q.head = 0;
    q.tail = 0;
    q.count = 0;

    for (int i = 1; i <= total_people; i++) {
        q.data[q.tail].id = i;
        q.tail = (q.tail + 1) % MAX_PEOPLE;
        q.count++;
    }

    while (q.count > 1) {
        for (int i = 1; i < report_interval; i++) {
            People p = q.data[q.head];
            q.head = (q.head + 1) % MAX_PEOPLE;
            q.count--;

            q.data[q.tail] = p;
            q.tail = (q.tail + 1) % MAX_PEOPLE;
            q.count++;
        }

        People out = q.data[q.head];
        q.head = (q.head + 1) % MAX_PEOPLE;
        q.count--;
        printf("淘汰: %d\n", out.id);
    }
    
    printf("最后剩下的人是: %d\n", q.data[q.head].id);

    return 0;
}
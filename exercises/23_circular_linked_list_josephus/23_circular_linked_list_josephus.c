#include <stdio.h>
#include <stdlib.h>

#include "circular_linked_list.h"

// 使用环形链表模拟约瑟夫环并打印出列顺序
static void josephus_problem(int n, int k, int m) {
    if (n <= 0 || k <= 0 || m <= 0) {
        printf("参数错误\n");
        return;
    }

    Node* head = create_circular_list(n);
    if (!head) {
        printf("\n");
        return;
    }

    // prev 指向 current 的前一个节点
    Node* current = head;
    Node* prev = head;
    while (prev->next != head) prev = prev->next;

    // 起始位置移动到第 k 个
    for (int i = 1; i < k; ++i) {
        prev = current;
        current = current->next;
    }

    // 执行约瑟夫环算法
    for (int i = 0; i < n; i++) {
        // 找到要删除的节点（第 m 个）
        if (m == 1) {
            // m=1，删除当前节点
            printf("%d ", current->id);
            Node* to_delete = current;
            prev->next = current->next;
            current = current->next;
            free(to_delete);
        } else {
            // 移动 m-1 步
            for (int j = 1; j < m; j++) {
                prev = current;
                current = current->next;
            }
            printf("%d ", current->id);
            Node* to_delete = current;
            prev->next = current->next;
            current = current->next;
            free(to_delete);
        }
    }

    printf("\n");
}

int main(void) {
    josephus_problem(5, 1, 2);  // 输出结果：2 4 1 5 3
    josephus_problem(7, 3, 1);  // 输出结果：3 4 5 6 7 1 2
    josephus_problem(9, 1, 8);  // 输出结果：8 7 9 2 5 4 1 6 3
    return 0;
}

/*
 * 循环链表实现 (用于约瑟夫环问题)
 *
 * 循环链表特性：
 * - 链表中最后一个节点的 next 指针指向头节点，形成一个环
 * - 从任意节点出发，都可以遍历整个链表
 * - 没有 NULL 结尾，需要特殊处理终止条件
 *
 * 与普通链表的区别：
 * - 普通链表：最后一个节点的 next = NULL
 * - 循环链表：最后一个节点的 next = head
 *
 * 图示：
 *   [节点1] -> [节点2] -> [节点3] -> ... -> [节点N]
 *     ^                                    |
 *     |____________________________________|
 *
 * 应用场景：
 * - 约瑟夫环问题
 * - 轮询调度算法
 * - 循环缓冲区的实现
 */

#include "circular_linked_list.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * 创建包含 n 个节点的循环链表
 *
 * @param n 要创建的节点数量
 * @return 循环链表的头节点指针，n<=0 时返回 NULL
 *
 * 创建步骤：
 * 1. 创建头节点 (id = 1)
 * 2. 依次创建后续节点 (id = 2 到 n)
 * 3. 将最后一个节点的 next 指向头节点，形成环
 *
 * 时间复杂度：O(n)
 */
Node* create_circular_list(int n) {
    if (n <= 0) return NULL;

    Node* head = malloc(sizeof(Node));
    head->id = 1;
    Node* current = head;

    for (int i = 2; i <= n; i++) {
        Node* new_node = malloc(sizeof(Node));
        new_node->id = i;
        current->next = new_node;
        current = new_node;
    }

    current->next = head;  // 将最后一个节点的 next 指向头节点，形成环
    return head;
}

/**
 * 释放循环链表的所有节点
 *
 * @param head 循环链表的头节点指针
 *
 * 释放步骤：
 * 1. 从头节点的下一个节点开始
 * 2. 依次释放每个节点，直到回到头节点
 * 3. 最后释放头节点
 *
 * @note 头节点是循环的"终点"，需要最后处理
 * 时间复杂度：O(n)
 */
void free_list(Node* head) {
    if (!head) return;

    Node* current = head->next;
    while (current != head) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    free(head);
}

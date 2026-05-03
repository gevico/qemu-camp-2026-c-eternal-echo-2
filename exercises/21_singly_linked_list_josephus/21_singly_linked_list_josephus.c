/*
 * 约瑟夫环问题 (单链表实现)
 *
 * 问题描述：
 * n 个人围成一圈，从某个指定的人开始报数，数到 m 的那个人出列。
 * 下一个人重新从 1 开始报数，直到所有人出列。
 * 求出列顺序。
 *
 * 算法分析：
 * - 时间复杂度：O(n * m)，n 为人数，m 为报数阈值
 * - 空间复杂度：O(n)
 *
 * 示例：
 * josephus_problem(5, 1, 2) 表示：
 * - 5 个人围成一圈 (编号 1-5)
 * - 从第 1 个人开始
 * - 每数到 2 个人出列
 * - 输出：2 4 1 5 3
 */

#include <stdio.h>
#include <stdlib.h>

#include "singly_linked_list.h"

/**
 * 打印节点的值
 *
 * @param p 链表节点指针
 */
void print_item(link p) { printf("%d ", p->item); }

/**
 * 通过遍历获取链表头节点
 *
 * 使用全局变量记录第一个访问的节点
 */
static link g_first_node = NULL;
static void capture_first(link p) {
    if (g_first_node == NULL) {
        g_first_node = p;
    }
}

static link get_head_node(void) {
    g_first_node = NULL;
    traverse(capture_first);
    return g_first_node;
}

/**
 * 获取下一个节点 (支持回绕)
 *
 * @param p 当前节点
 * @return 下一个节点，如果到达尾部则返回头节点
 *
 * @note 此函数将单链表模拟为循环链表
 */
static inline link next_wrap(link p) {
    if (p == NULL) return get_head_node();
    return p->next ? p->next : get_head_node();
}

/**
 * 创建包含 n 个节点的单链表
 *
 * @param n 节点数量
 *
 * @note 节点按降序创建 (n, n-1, ..., 1)，使用 push 函数
 */
void create_list(int n) {
    // 参数校验
    if (n <= 0) return;

    destroy();
    for (int i = n; i >= 1; i--) {
        link new_node = make_node(i);
        push(new_node);
    }
}

/**
 * 约瑟夫环问题求解
 *
 * @param n 总人数 (节点数)
 * @param k 起始位置 (从 1 开始计数)
 * @param m 报数阈值 (数到 m 的人出列)
 *
 * 算法步骤：
 * 1. 创建包含 n 个节点的单链表
 * 2. 移动到第 k 个位置作为起点
 * 3. 循环执行直到所有人出列：
 *    a. 从当前节点开始走 m-1 步
 *    b. 删除第 m 个节点
 *    c. 打印出列的人
 *    d. 从下一个人重新开始计数
 *
 * 特殊情况处理：
 * - m = 1：当前节点直接出列，无需移动
 * - k > n：支持超范围，通过回绕实现
 */
void josephus_problem(int n, int k, int m) {
    // 参数校验
    if (n <= 0 || k <= 0 || m <= 0) {
        printf("参数错误：n, k, m 都必须大于 0\n");
        return;
    }

    create_list(n);

    // 从头开始，移动到第 k 个位置作为起点（支持 k > n 的情况，按环形回绕）
    link current = get_head_node();
    if (!current) {
        printf("\n");
        return;
    }
    for (int i = 1; i < k; ++i) {
        current = next_wrap(current);
    }

    // 依次出列并打印顺序
    for (int out = 0; out < n; ++out) {
        link to_delete = current;

        if (m == 1) {
            // m==1 时当前节点直接出列
            current = next_wrap(current);
        } else {
            // 数到 m 的那个人出列：从 current 开始走 m-1 步，落在第 m 个节点的前一个
            for (int i = 1; i < m - 1; ++i) {
                current = next_wrap(current);
            }

            // 要删除的节点是 current 的下一个节点
            to_delete = next_wrap(current);

            // 将 current 指向要删除节点的下一个节点
            if (current->next == to_delete) {
                current->next = to_delete->next;
            } else if (current == to_delete) {
                // 只有一个节点的情况
                current = NULL;
            } else {
                // 需要找到链表中 to_delete 的前一个节点
                link temp = get_head_node();
                while (temp && temp->next != to_delete && temp->next != NULL) {
                    temp = temp->next;
                }
                if (temp && temp->next == to_delete) {
                    temp->next = to_delete->next;
                }
            }

            current = next_wrap(to_delete);
        }

        // 打印出列的人
        print_item(to_delete);
    }

    printf("\n");
}

/**
 * 主函数：约瑟夫环问题测试
 *
 * @return 程序退出状态码
 *
 * 测试用例：
 * 1. josephus_problem(5, 1, 2): 5人，从第1人开始，数到2出列
 * 2. josephus_problem(7, 3, 1): 7人，从第3人开始，数到1出列
 * 3. josephus_problem(9, 1, 8): 9人，从第1人开始，数到8出列
 */
int main(void) {
    josephus_problem(5, 1, 2);  // 输出结果：2 4 1 5 3
    josephus_problem(7, 3, 1);  // 输出结果：3 4 5 6 7 1 2
    josephus_problem(9, 1, 8);  // 输出结果：8 7 9 2 5 4 1 6 3

    return 0;
}

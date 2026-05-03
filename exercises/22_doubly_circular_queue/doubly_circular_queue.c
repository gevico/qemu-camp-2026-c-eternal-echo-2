/*
 * 双向循环队列实现
 *
 * 数据结构：使用带头尾哨兵节点的双向循环链表
 *
 * 工作原理：
 * - 哨兵节点：head 和 tail 是不存储数据的特殊节点，简化边界处理
 * - 循环特性：最后一个节点的 next 指向第一个节点，第一个节点的 prev 指向最后一个节点
 * - 双向链表：每个节点同时维护前驱和后继指针，支持双向遍历
 *
 * 优势：
 * - O(1) 时间复杂度的插入和删除操作
 * - 无需检查 NULL 指针，代码更简洁
 * - 支持在队列两端快速操作
 *
 * 图示：
 *   NULL <-[head哨兵]<=>[节点1]<=>[节点2]<=>...<=>[节点N]<=>[tail哨兵]->NULL
 *            ^                                                    ^
 *            |________________prev_______________________________|
 */

#include "doubly_circular_queue.h"

#include <stdlib.h>

// 哨兵节点声明
// 哨兵节点简化了链表操作，无需处理边界条件
static struct node tailsentinel;
static struct node headsentinel = {0, NULL, &tailsentinel};
static struct node tailsentinel = {0, &headsentinel, NULL};

// 全局头尾指针，始终指向哨兵节点
static link head = &headsentinel;
static link tail = &tailsentinel;

/**
 * 创建新节点
 *
 * @param data 要存储的数据
 * @return 新分配的节点指针，失败返回 NULL
 *
 * @note 新节点的 prev 和 next 指针初始化为 NULL
 */
link make_node(int data) {
    link p = malloc(sizeof(*p));
    if (p) {
        p->data = data;
        p->prev = p->next = NULL;
    }
    return p;
}

/**
 * 释放节点内存
 *
 * @param p 要释放的节点指针
 */
void free_node(link p) {
    free(p);
}

/**
 * 在队列中查找指定数据的节点
 *
 * @param key 要查找的数据值
 * @return 找到的节点指针，未找到返回 NULL
 *
 * 查找范围：从 head 的下一个节点开始，到 tail 的前一个节点结束
 * 时间复杂度：O(n)
 */
link search(int key) {
    for (link p = head->next; p != tail; p = p->next) {
        if (p->data == key) {
            return p;
        }
    }
    return NULL;
}

/**
 * 在队列头部插入节点
 *
 * 插入位置：head 哨兵之后
 *
 * @param p 要插入的节点指针
 *
 * 插入步骤：
 * 1. 将新节点的 next 指向原第一个节点
 * 2. 将新节点的 prev 指向 head
 * 3. 更新原第一个节点的 prev 指向新节点
 * 4. 更新 head 的 next 指向新节点
 *
 * 时间复杂度：O(1)
 */
void insert(link p) {
    p->next = head->next;
    p->prev = head;
    head->next->prev = p;
    head->next = p;
}

/**
 * 从队列中删除节点
 *
 * @param p 要删除的节点指针
 *
 * 删除步骤：
 * 1. 将前驱节点的 next 指向当前节点的后继
 * 2. 将后继节点的 prev 指向当前节点的前驱
 * 3. 当前节点从链表中脱离
 *
 * @note 删除操作不释放节点内存，需调用 free_node
 * 时间复杂度：O(1)
 */
void delete(link p) {
    p->prev->next = p->next;
    p->next->prev = p->prev;
}

/**
 * 遍历队列中的所有节点
 *
 * @param visit 访问函数指针，对每个节点执行此操作
 *
 * 遍历范围：从 head 的下一个节点到 tail 的前一个节点
 */
void traverse(void (*visit)(link)) {
    for (link p = head->next; p != tail; p = p->next) {
        visit(p);
    }
}

/**
 * 销毁队列，释放所有节点内存
 *
 * 释放所有数据节点，但保留哨兵节点
 * 释放后队列恢复到初始空状态
 *
 * 时间复杂度：O(n)
 */
void destroy(void) {
    link p = head->next;
    while (p != tail) {
        link next = p->next;
        free_node(p);
        p = next;
    }
    // 重置哨兵指针，使队列为空
    head->next = tail;
    tail->prev = head;
}

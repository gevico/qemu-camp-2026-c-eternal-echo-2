/*
 * 二叉树实现 (使用队列构建)
 *
 * 数据结构：二叉树 + 队列
 *
 * 二叉树特性：
 * - 每个节点最多有两个子节点 (左子节点和右子节点)
 * - 层序构建：按照层级从上到下、从左到右构建树
 *
 * 树的遍历方式：
 * 1. 前序遍历 (根-左-右)：递归实现 vs 迭代实现
 * 2. 中序遍历 (左-根-右)
 * 3. 后序遍历 (左-右-根)
 * 4. 层序遍历 (BFS)
 *
 * 本程序实现：
 * - 使用队列按层序构建二叉树
 * - 前序遍历的递归和迭代两种实现
 */

#include "simple_tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/**
 * 创建空队列
 *
 * @return 新队列的指针，失败返回 NULL
 */
Queue* create_queue() {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

/**
 * 入队操作：将树节点添加到队列尾部
 *
 * @param q 队列指针
 * @param tree_node 要入队的树节点
 *
 * 时间复杂度：O(1)
 */
void enqueue(Queue *q, TreeNode *tree_node) {
    QueueNode *new_node = (QueueNode*)malloc(sizeof(QueueNode));
    new_node->tree_node = tree_node;
    new_node->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = new_node;
    } else {
        q->rear->next = new_node;
        q->rear = new_node;
    }
}

/**
 * 出队操作：从队列头部移除并返回树节点
 *
 * @param q 队列指针
 * @return 出队的树节点，队列为空时返回 NULL
 *
 * 时间复杂度：O(1)
 */
TreeNode* dequeue(Queue *q) {
    if (is_empty(q)) {
        return NULL;
    }

    QueueNode *temp = q->front;
    TreeNode *tree_node = temp->tree_node;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return tree_node;
}

/**
 * 检查队列是否为空
 *
 * @param q 队列指针
 * @return 如果队列为空返回 true，否则返回 false
 */
bool is_empty(Queue *q) {
    return q->front == NULL;
}

/**
 * 释放队列内存
 *
 * @param q 队列指针
 *
 * @note 只释放队列结构本身，不释放队列中存储的树节点
 */
void free_queue(Queue *q) {
    while (!is_empty(q)) {
        dequeue(q);
    }
    free(q);
}

/**
 * 按层序构建二叉树
 *
 * 根据给定的层序遍历数组构建二叉树
 *
 * @param level_order 层序遍历数组，INT_MIN 表示空节点
 * @param size 数组大小
 * @return 构建的二叉树根节点
 *
 * 构建算法 (BFS)：
 * 1. 创建根节点并入队
 * 2. 循环处理队列中的每个节点：
 *    a. 从数组中读取两个值作为当前节点的左右子节点
 *    b. 如果值不是 INT_MIN，创建节点并入队
 * 3. 直到数组处理完毕或队列为空
 *
 * 时间复杂度：O(n)
 * 空间复杂度：O(n) (队列存储)
 */
TreeNode* build_tree_by_level(int *level_order, int size) {
    if (size == 0 || level_order[0] == INT_MIN) {
        return NULL;
    }

    TreeNode *root = (TreeNode*)malloc(sizeof(TreeNode));
    root->val = level_order[0];
    root->left = root->right = NULL;

    Queue *q = create_queue();
    enqueue(q, root);

    int i = 1;
    while (!is_empty(q) && i < size) {
        TreeNode *current = dequeue(q);

        if (i < size && level_order[i] != INT_MIN) {
            current->left = (TreeNode*)malloc(sizeof(TreeNode));
            current->left->val = level_order[i];
            current->left->left = current->left->right = NULL;
            enqueue(q, current->left);
        }
        i++;

        if (i < size && level_order[i] != INT_MIN) {
            current->right = (TreeNode*)malloc(sizeof(TreeNode));
            current->right->val = level_order[i];
            current->right->left = current->right->right = NULL;
            enqueue(q, current->right);
        }
        i++;
    }

    free_queue(q);
    return root;
}

/**
 * 前序遍历 (递归实现)
 *
 * 遍历顺序：根节点 -> 左子树 -> 右子树
 *
 * @param root 二叉树根节点
 *
 * 时间复杂度：O(n)
 * 空间复杂度：O(h)，h 为树的高度 (递归栈)
 */
void preorder_traversal(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    printf("%d ", root->val);
    preorder_traversal(root->left);
    preorder_traversal(root->right);
}

/**
 * 前序遍历 (迭代实现)
 *
 * 使用栈模拟递归过程，避免递归栈的开销
 *
 * @param root 二叉树根节点
 *
 * 算法步骤：
 * 1. 将根节点压入栈
 * 2. 循环执行直到栈为空：
 *    a. 弹出栈顶节点并访问
 *    b. 先压入右子节点 (后访问)
 *    c. 再压入左子节点 (先访问)
 *
 * 时间复杂度：O(n)
 * 空间复杂度：O(h)，h 为树的高度 (栈空间)
 */
void preorder_traversal_iterative(TreeNode *root) {
    if (root == NULL) {
        return;
    }

    TreeNode *stack[1000];
    int top = -1;

    stack[++top] = root;

    while (top >= 0) {
        TreeNode *node = stack[top--];
        printf("%d ", node->val);

        if (node->right != NULL) {
            stack[++top] = node->right;
        }
        if (node->left != NULL) {
            stack[++top] = node->left;
        }
    }
}

/**
 * 释放二叉树内存 (后序递归释放)
 *
 * 先释放左右子树，最后释放根节点
 *
 * @param root 二叉树根节点
 *
 * 时间复杂度：O(n)
 * 空间复杂度：O(h)，h 为树的高度 (递归栈)
 */
void free_tree(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

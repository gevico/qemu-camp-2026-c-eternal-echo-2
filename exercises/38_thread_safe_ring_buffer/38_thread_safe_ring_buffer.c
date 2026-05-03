/*
 * === 线程安全环形缓冲区 ===
 *
 * 环形缓冲区是一种固定大小的 FIFO 数据结构
 * 当缓冲区满时，新数据会覆盖最旧的数据 (或等待)
 *
 * 本实现特点：
 * - 使用互斥锁 (pthread_mutex_t) 保护共享数据
 * - 使用条件变量 (pthread_cond_t) 实现线程间通信
 * - 支持多生产者、多消费者模型
 *
 * 线程同步机制：
 * 1. not_full 条件：缓冲区不满时，唤醒生产者
 * 2. not_empty 条件：缓冲区不空时，唤醒消费者
 *
 * 工作流程：
 * - 生产者：获取锁 -> 等待 not_full -> 写入数据 -> 唤醒消费者 -> 释放锁
 * - 消费者：获取锁 -> 等待 not_empty -> 读取数据 -> 唤醒生产者 -> 释放锁
 *
 * 图示：
 *   [0][1][2][3][4]
 *    ^        ^
 *   head     tail
 *   读指针   写指针
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 数据结构定义
 */

typedef struct {
    int *buf;                 /* 数据缓冲区（整数队列） */
    size_t capacity;          /* 容量（元素个数） */
    size_t head;              /* 读指针 */
    size_t tail;              /* 写指针 */
    size_t count;             /* 当前元素个数 */
    pthread_mutex_t mtx;      /* 互斥锁 */
    pthread_cond_t not_full;  /* 条件：非满 */
    pthread_cond_t not_empty; /* 条件：非空 */
} ring_buffer_t;

/**
 * 初始化环形缓冲区
 *
 * @param rb 环形缓冲区指针
 * @param capacity 缓冲区容量 (元素个数)
 * @return 成功返回 0，失败返回 -1
 *
 * 初始化步骤：
 * 1. 分配数据缓冲区
 * 2. 初始化互斥锁
 * 3. 初始化两个条件变量
 */
static int rb_init(ring_buffer_t *rb, size_t capacity) {
    rb->buf = (int *)malloc(capacity * sizeof(int));
    if (!rb->buf) {
        return -1;
    }

    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;

    if (pthread_mutex_init(&rb->mtx, NULL) != 0) {
        free(rb->buf);
        return -1;
    }

    if (pthread_cond_init(&rb->not_full, NULL) != 0) {
        pthread_mutex_destroy(&rb->mtx);
        free(rb->buf);
        return -1;
    }

    if (pthread_cond_init(&rb->not_empty, NULL) != 0) {
        pthread_cond_destroy(&rb->not_full);
        pthread_mutex_destroy(&rb->mtx);
        free(rb->buf);
        return -1;
    }

    return 0;
}

/**
 * 销毁环形缓冲区
 *
 * @param rb 环形缓冲区指针
 *
 * 释放顺序：
 * 1. 销毁条件变量
 * 2. 销毁互斥锁
 * 3. 释放数据缓冲区
 */
static void rb_destroy(ring_buffer_t *rb) {
    pthread_mutex_destroy(&rb->mtx);
    pthread_cond_destroy(&rb->not_full);
    pthread_cond_destroy(&rb->not_empty);
    free(rb->buf);
}

/**
 * 入队操作 (线程安全)
 *
 * @param rb 环形缓冲区指针
 * @param val 要入队的值
 *
 * 线程同步流程：
 * 1. 获取互斥锁
 * 2. 如果缓冲区满，等待 not_full 条件
 * 3. 写入数据到 tail 位置
 * 4. 移动 tail 指针 (循环)
 * 5. 增加 count 计数
 * 6. 发送 not_empty 信号 (可能有消费者在等待)
 * 7. 释放互斥锁
 */
static void rb_push(ring_buffer_t *rb, int val) {
    pthread_mutex_lock(&rb->mtx);

    while (rb->count >= rb->capacity) {
        pthread_cond_wait(&rb->not_full, &rb->mtx);
    }

    rb->buf[rb->tail] = val;
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->count++;

    pthread_cond_signal(&rb->not_empty);
    pthread_mutex_unlock(&rb->mtx);
}

/**
 * 出队操作 (线程安全)
 *
 * @param rb 环形缓冲区指针
 * @param out 输出参数，存储出队的值
 * @return 成功返回 0
 *
 * 线程同步流程：
 * 1. 获取互斥锁
 * 2. 如果缓冲区空，等待 not_empty 条件
 * 3. 从 head 位置读取数据
 * 4. 移动 head 指针 (循环)
 * 5. 减少 count 计数
 * 6. 发送 not_full 信号 (可能有生产者在等待)
 * 7. 释放互斥锁
 */
static int rb_pop(ring_buffer_t *rb, int *out) {
    pthread_mutex_lock(&rb->mtx);

    while (rb->count == 0) {
        pthread_cond_wait(&rb->not_empty, &rb->mtx);
    }

    *out = rb->buf[rb->head];
    rb->head = (rb->head + 1) % rb->capacity;
    rb->count--;

    pthread_cond_signal(&rb->not_full);
    pthread_mutex_unlock(&rb->mtx);

    return 0;
}

typedef struct {
    ring_buffer_t *rb;
    const int *data;
    size_t n;
} producer_arg_t;

typedef struct {
    ring_buffer_t *rb;
    size_t n;
} consumer_arg_t;

static void *producer(void *arg) {
    producer_arg_t *pa = (producer_arg_t *)arg;

    for (size_t i = 0; i < pa->n; i++) {
        rb_push(pa->rb, pa->data[i]);
    }

    return NULL;
}

static void *consumer(void *arg) {
    consumer_arg_t *ca = (consumer_arg_t *)arg;

    for (size_t i = 0; i < ca->n; i++) {
        int val;
        rb_pop(ca->rb, &val);
        if (i == ca->n - 1) {
            printf("%d\n", val);
        } else {
            printf("%d,", val);
        }
    }

    return NULL;
}

int main(void) {
    /* 输入：缓冲区容量 5，生产者线程写入 [1,2,3,4,5,6]（第 6 个元素等待消费者读取后写入）
     * 输出：消费者打印 1,2,3,4,5,6（顺序正确，无数据丢失）。
     */
    setvbuf(stdout, NULL, _IOLBF, 0); /* 行缓冲，便于在测试中看到输出 */

    ring_buffer_t rb;
    if (rb_init(&rb, 5) != 0) {
        fprintf(stderr, "ring buffer init failed\n");
        return 1;
    }

    const int data[] = {1, 2, 3, 4, 5, 6};
    const size_t n = sizeof(data) / sizeof(data[0]);

    pthread_t th_p, th_c;
    producer_arg_t pa = {.rb = &rb, .data = data, .n = n};
    consumer_arg_t ca = {.rb = &rb, .n = n};

    if (pthread_create(&th_c, NULL, consumer, &ca) != 0) {
        fprintf(stderr, "pthread_create consumer failed\n");
        rb_destroy(&rb);
        return 1;
    }
    if (pthread_create(&th_p, NULL, producer, &pa) != 0) {
        fprintf(stderr, "pthread_create producer failed\n");
        rb_destroy(&rb);
        return 1;
    }

    pthread_join(th_p, NULL);
    pthread_join(th_c, NULL);

    rb_destroy(&rb);
    return 0;
}

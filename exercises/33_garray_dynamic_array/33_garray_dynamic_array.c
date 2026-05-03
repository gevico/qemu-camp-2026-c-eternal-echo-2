#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 13 可扩展动态数组（类似 QEMU GArray）
 * 要求：
 *  - 实现动态数组 GArray，初始容量 16，满时按 2 倍扩容
 *  - 接口：
 *      GArray* garray_init(size_t elem_size);
 *      void    garray_append(GArray* arr, void* elem);
 *      void    garray_free(GArray* arr);
 *  - 测试：以 int 作为元素类型，追加 17 个元素后，len=17，capacity=32，数据正确
 */

#define GARRAY_INIT_CAP 16U

/*
 * 结构体定义：为实现泛型 append 的字节拷贝，必须保存元素大小 elem_size
 */
typedef struct {
    void* data;       /* 数据缓冲区 */
    size_t len;       /* 已使用的元素个数 */
    size_t capacity;  /* 当前容量（元素个数） */
    size_t elem_size; /* 元素大小（字节数） */
} GArray;

/* 接口：初始化动态数组 */
GArray* garray_init(size_t elem_size) {
    GArray* arr = (GArray*)malloc(sizeof(GArray));
    if (!arr) {
        return NULL;
    }

    arr->data = malloc(GARRAY_INIT_CAP * elem_size);
    if (!arr->data) {
        free(arr);
        return NULL;
    }

    arr->len = 0;
    arr->capacity = GARRAY_INIT_CAP;
    arr->elem_size = elem_size;
    return arr;
}

/* 接口：追加单个元素，必要时扩容为原来的 2 倍 */
void garray_append(GArray* arr, void* elem) {
    if (!arr || !elem) {
        return;
    }

    // Check if we need to expand
    if (arr->len >= arr->capacity) {
        size_t new_capacity = arr->capacity * 2;
        void* new_data = realloc(arr->data, new_capacity * arr->elem_size);
        if (!new_data) {
            return;  // Expansion failed
        }
        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    // Copy the element to the end of the array
    void* dest = (char*)arr->data + (arr->len * arr->elem_size);
    memcpy(dest, elem, arr->elem_size);
    arr->len++;
}

/* 接口：释放动态数组 */
void garray_free(GArray* arr) {
    if (!arr) {
        return;
    }
    free(arr->data);
    free(arr);
}

int main(void) {
    GArray* a = garray_init(sizeof(int));
    if (!a) {
        fprintf(stderr, "初始化失败\n");
        return 1;
    }

    // 追加 1..17 共 17 个元素（触发一次扩容 16->32）
    for (int i = 1; i <= 17; ++i) {
        garray_append(a, &i);
    }
    // 打印 len/capacity 以及关键元素值，便于自动化判定
    printf("len=%zu\n", a->len);
    printf("capacity=%zu\n", a->capacity);
    int* arr_i = (int*)a->data;
    printf("arr[0]=%d\n", arr_i[0]);
    printf("arr[16]=%d\n", arr_i[16]);

    // 校验：len==17 && capacity==32 && 首尾值正确
    int ok = (a->len == 17 && a->capacity == 32 && arr_i[0] == 1 && arr_i[16] == 17);
    if (!ok) {
        garray_free(a);
        return 1;
    }

    garray_free(a);
    return 0;
}

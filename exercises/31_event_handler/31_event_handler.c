#include <stddef.h>
#include <stdio.h>

/*
 * 11 简单事件处理器（回调机制）
 * 要求：
 *  - 事件类型用枚举定义（enum { EVENT_A, EVENT_B, EVENT_MAX }）。
 *  - 注册函数：void register_event(enum EVENT_TYPE type, void (*callback)(void*), void* arg)。
 *  - 触发函数：void trigger_event(enum EVENT_TYPE type)。
 *  - 测试：注册 EVENT_A 的回调打印 "Event A triggered"，触发后应输出该信息
 */

/* 定义事件类型枚举 */
enum EVENT_TYPE { EVENT_A = 0, EVENT_B, EVENT_MAX };

/* 定义回调函数类型 */
typedef void (*event_callback_t)(void* arg);

/*
 * 使用两个数组分别保存每个事件类型的回调函数和其参数
 * 索引与 enum EVENT_TYPE 对应
 */
static event_callback_t g_callbacks[EVENT_MAX] = {0};
static void* g_callback_args[EVENT_MAX] = {0};

/*
 * 注册事件函数：为指定事件类型设置回调与参数
 */
void register_event(enum EVENT_TYPE type, void (*callback)(void*), void* arg) {
    if (type >= 0 && type < EVENT_MAX) {
        g_callbacks[type] = callback;
        g_callback_args[type] = arg;
    }
}

/*
 * 触发事件函数：若已注册回调则调用
 */
void trigger_event(enum EVENT_TYPE type) {
    if (type >= 0 && type < EVENT_MAX && g_callbacks[type] != NULL) {
        g_callbacks[type](g_callback_args[type]);
    }
}

/*
 * 测试函数：注册并触发 EVENT_A
 */
static void on_event_a(void* arg) {
    const char* msg = (const char*)arg;
    if (msg) {
        printf("%s\n", msg);
    }
}

int main(void) {
    /* 期待输出：Event A triggered */
    const char* msg = "Event A triggered";
    register_event(EVENT_A, on_event_a, (void*)msg);
    trigger_event(EVENT_A);
    return 0;
}

#include <stdio.h>

// TODO：定义四则运算规则
// hint：似乎除数有一些特殊情况，需要处理

/**
 * @brief 加法运算。
 */
int add(int a, int b) { return a + b; }

/**
 * @brief 减法运算。
 */
int subtract(int a, int b) { return a - b; }

/**
 * @brief 乘法运算。
 */
int multiply(int a, int b) { return a * b; }

/**
 * @brief 除法运算，处理除数为 0 的错误场景。
 *
 * @note 当 b == 0 时打印错误并返回 0，避免程序崩溃。
 */
int divide(int a, int b) {
    if (b == 0) {
        printf("错误：除数不能为零\n");
        return 0;
    }
    return a / b;
}



int main() {
    int a, b;
    char op;

    // 使用函数指针数组将运算符映射到具体函数实现
    int (*operations[])(int, int) = {add, subtract, multiply, divide};

    char operators[] = "+-*/";

    printf("输入两个整数和一个运算符 (+, -, *, /): ");
    scanf("%d %d %c", &a, &b, &op);

    // 查找对应的运算符索引
    int index = -1;
    for (int i = 0; i < 4; i++) {
        if (op == operators[i]) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("无效的运算符\n");
        return 1;
    }

    int result = operations[index](a, b);
    printf("%d %c %d = %d\n", a, op, b, result);

    return 0;
}
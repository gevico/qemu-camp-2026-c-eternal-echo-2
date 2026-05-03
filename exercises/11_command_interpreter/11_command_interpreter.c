#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARGS 10
#define MAX_INPUT_LENGTH 256

int shell_parse(char *buf, char *argv[]);
void execute_command(int argc, char *argv[]);

// in:  format -> hello world "hello world" 1234567
// Multiple command parameters entered from the command line (number of parameters does not exceed 10)
// out:  format -> Parameter X: Content, Length: X
// Parse the command parameters through a character pointer array, and sequentially display the content and length of each parameter on the screen
//
int main(void)
{
    FILE *file;
    char input[MAX_INPUT_LENGTH];
    char *argv[MAX_ARGS + 1] = {NULL};
    int argc = 0;

    file = fopen("command_file.txt", "r");
    if (!file) {
        fprintf(stderr, "❌ Error: Cannot open input file '%s'\n", "command_file.txt");
        return 1;
    }

    printf("✅ Reading commands from '%s':\n\n", "command_file.txt");

    while (fgets(input, MAX_INPUT_LENGTH, file) != NULL) {
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0 || strspn(input, " \t") == strlen(input)) {
            continue;
        }

        printf("➡️  Input: %s\n", input);

        argc = shell_parse(input, argv);

        if (argc == 0) {
            printf("⚠️  No valid command parsed.\n\n");
            continue;
        }

        execute_command(argc, argv);
        printf("\n");
    }

    fclose(file);
    return 0;
}

// shell_parse 和 execute_command 保持不变
int shell_parse(char *buf, char *argv[])
{
    int argc = 0;
    int state = 0;
    /**
     * 状态机解析命令行参数：
     * state=0: 空闲状态（当前不在参数中）
     * state=1: 普通参数中（以空格/制表符结束）
     * state=2: 双引号参数中（以双引号结束）
     *
     * 解析方式为“原地切分”：
     * - 在参数结束位置写入 '\0'
     * - argv[i] 指向每个参数起始地址
     */
    char *p = buf;

    while (*p != '\0' && argc < MAX_ARGS) {
        if (state == 0) {
            if (*p == ' ' || *p == '\t') {
                p++;
            } else if (*p == '"') {
                state = 2;
                argv[argc++] = p + 1;  // 跳过开引号
                p++;
            } else {
                state = 1;
                argv[argc++] = p;
                p++;
            }
        } else if (state == 1) {
            if (*p == ' ' || *p == '\t') {
                *p = '\0';
                state = 0;
            }
            p++;
        } else {  // state == 2
            if (*p == '"') {
                *p = '\0';
                state = 0;
            }
            p++;
        }
    }

    // 若字符串结束时仍在普通参数中，参数天然已以 '\0' 结束
    if (state == 2) {
        // 未闭合引号：按整段参数处理，保持兼容性
    }

    argv[argc] = NULL;
    return argc;
}

void execute_command(int argc, char *argv[])
{
    printf("Parsing result: Total %d parameters\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("Parameter %d: Content: %s, Length: %zu\n",
               i + 1, argv[i], strlen(argv[i]));
    }

    if (strcmp(argv[0], "help") == 0) {
        printf("This is help command\n");
    } else if (strcmp(argv[0], "echo") == 0) {
        printf("Echo: ");
        for (int i = 1; i < argc; i++) {
            printf("%s ", argv[i]);
        }
        printf("\n");
    } else if (strcmp(argv[0], "add") == 0 && argc == 3) {
        int a = atoi(argv[1]);
        int b = atoi(argv[2]);
        printf("%d + %d = %d\n", a, b, a + b);
    } else {
        printf("Unknown command: %s\n", argv[0]);
    }
}

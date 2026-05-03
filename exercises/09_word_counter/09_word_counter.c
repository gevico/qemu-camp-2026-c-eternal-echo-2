#include <stdio.h>

int main() {
    char str[]="Don't ask what your country can do for you, but ask what you can do for your country.";
    int wordCount = 0;
    int inWord = 0;

    /**
     * 以空格和换行为分隔符进行单词计数：
     * - 从“非单词状态”进入“单词状态”时计数 +1
     * - 遇到分隔符时回到“非单词状态”
     */
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ' && str[i] != '\n' && !inWord) {
            inWord = 1;
            wordCount++;
        } else if (str[i] == ' ' || str[i] == '\n') {
            inWord = 0;
        }
    }
    
    printf("单词数量: %d\n", wordCount);
    
    return 0;
}
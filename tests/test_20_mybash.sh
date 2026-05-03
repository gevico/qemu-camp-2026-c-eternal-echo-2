#!/bin/bash

cd ../exercises/20_mybash

# 复制 dict.txt 到 mytrans 目录，如果不存在
if [ ! -f src/mytrans/dict.txt ]; then
    if [ -f ../../19_mytrans/dict.txt ]; then
        cp ../../19_mytrans/dict.txt src/mytrans/
    fi
fi

# 运行 mybash 并传入测试命令
cat << 'EOF' | ./bin/mybash /dev/stdin
myfile ./bin/mybash
myfile ./obj/mybash.o
mysed s/unix/linux/ "unix is opensource. unix is free os."
mytrans ./src/mytrans/text.txt
mywc ./src/mytrans/text.txt
EOF

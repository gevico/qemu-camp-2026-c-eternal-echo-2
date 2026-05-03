# =============================================
# 主 Makefile
# 功能：
#   - mybash：生成二进制，放到 bin/
#   - 其他模块（mysed, myfile, mywc, mytrans）：生成静态库 .a，放到 obj/xxx/，.o 放到 obj/xxx/（或 out/，但建议统一）
#   - 所有子模块 Makefile 应支持 OBJDIR 和 BINDIR 参数
# =============================================

PWD := $(shell pwd)

# 所有子模块目录（静态库：生成 .a）
SUBDIRS := \
	mysed \
	myfile \
	mywc \
	mytrans

# 默认目标：构建 mybash（依赖其他库）
all: mybash

# 通用规则：构建每个子模块（静态库）
$(SUBDIRS):
	$(MAKE) -C src/$@

# 构建 mybash（可执行文件，依赖上面的库）
mybash: $(SUBDIRS)
	mkdir -p bin obj
	gcc -Wall -g -O2 -c src/mybash/main.c -o obj/mybash.o -Iinclude
	gcc -Wall -g -O2 -o bin/mybash obj/mybash.o \
		-Lsrc/mysed/obj -Lsrc/myfile/obj -Lsrc/mywc/obj -Lsrc/mytrans/obj \
		-lmysed -lmyfile -lmywc -lmytrans

# 清理所有生成的文件
clean:
	rm -rf bin obj
	for dir in $(SUBDIRS); do \
		$(MAKE) -C src/$$dir clean; \
	done

.PHONY: all $(SUBDIRS) mybash clean
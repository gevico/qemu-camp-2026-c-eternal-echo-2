# src/mytrans/Makefile

CC = gcc
CFLAGS = -Wall -I../../include

# 库名称
LIBNAME := mytrans
# 静态库输出名：libmytrans.a
LIB := lib$(LIBNAME).a
# 最终库输出目录（比如 ./obj/src/mytrans/）
OBJDIR ?= ./obj
# 最终 .a 文件路径
LIB_PATH := $(OBJDIR)/$(LIB)

# 源文件
SRCS := mytrans.c myhash.c
OBJS := $(SRCS:.c=.o)
OBJS := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJS))

all: $(LIB_PATH)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_PATH): $(OBJS)
	ar rcs $@ $^

clean:
	@rm -rf $(OBJDIR)/*.o $(LIB_PATH)
# 使用 POSIX 线程 API（pthread_create/mutex/cond 等）
CC = gcc
CPPFLAGS ?=
CPPFLAGS += -MMD -MP
CPPFLAGS += -pthread
CFLAGS = -Wall -Wextra -O2 -std=c11 -pthread
LDFLAGS =
LDLIBS = -pthread

TARGET = 38_thread_safe_ring_buffer
SRC = 38_thread_safe_ring_buffer.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

.PHONY: all clean run rebuild debug check

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

run: all
	@./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ) $(DEP)

rebuild: clean all

debug: CFLAGS += -g -O0
debug: clean all

check: all
	@./$(TARGET)

-include $(DEP)

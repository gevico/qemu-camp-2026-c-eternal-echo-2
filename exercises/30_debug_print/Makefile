CC = gcc
CPPFLAGS ?=
CPPFLAGS += -MMD -MP
CFLAGS = -Wall
LDFLAGS =
LDLIBS =
DEBUG_LEVEL ?= 2

TARGET = 30_debug_print
SRC = 30_debug_print.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

.PHONY: all clean run rebuild debug check

CPPFLAGS += -DDEBUG_LEVEL=$(DEBUG_LEVEL)

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

check: run

-include $(DEP)

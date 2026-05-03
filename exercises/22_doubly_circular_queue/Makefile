CC = gcc
CPPFLAGS ?=
CPPFLAGS += -MMD -MP
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS =
LDLIBS =

TARGET = 22_doubly_circular_queue
SRC = 22_doubly_circular_queue.c doubly_circular_queue.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

.PHONY: all clean run rebuild debug check

all: $(TARGET)

check: $(TARGET)
	./$(TARGET)

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

-include $(DEP)

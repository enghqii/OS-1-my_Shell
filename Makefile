
TARGETS := my_shell

MY_SHELL_OBJS := my_shell.o

OBJS := $(MY_SHELL_OBJS)

CC := gcc

CFLAGS += -D_REENTRANT -D_LIBC_REENTRANT -D_THREAD_SAFE
CFLAGS += -Wall
CFLAGS += -Wunused
CFLAGS += -Wshadow
CFLAGS += -Wdeclaration-after-statement
CFLAGS += -Wdisabled-optimization
CFLAGS += -Wpointer-arith
CFLAGS += -Wredundant-decls
CFLAGS += -g -O2

LDFLAGS +=

%.o: %.c
	$(CC) -o $*.o $< -c $(CFLAGS)

.PHONY: all clean test

all: $(TARGETS)

clean:
	-rm -f $(TARGETS) $(OBJS) *~ *.bak core*


my_shell: $(SCHED_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

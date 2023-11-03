SRCS = makeargv.c ush.c
PROG = ush
CC = gcc
CFLAGS = -I./include -lm -g -Wall
OBJS = $(SRCS:.c=.o)

$(PROG): $(OBJS)
	$(CC)  -o $@ $^ $(CFLAGS)

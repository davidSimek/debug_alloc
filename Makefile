CC = gcc
CFLAGS = -Wall -g
TARGET = out/run
SRCS = src/main.c
HDRS = src/debug_alloc.h
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)


CC = gcc
CFLAGS = -Wall -g
TARGET = out/run
SRCS = src/main.c src/debug_alloc.c
HDRS = src/debug_alloc.h
OBJS = obj/main.o obj/debug_alloc.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: src/%.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)


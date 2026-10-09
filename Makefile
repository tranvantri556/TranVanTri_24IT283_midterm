CC      = gcc
CFLAGS  = -Wall -Wextra -std=gnu11 -D_DEFAULT_SOURCE
OBJS    = main.o options.o list.o format.o util.o
TARGET  = myls

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

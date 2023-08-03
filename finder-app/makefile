CC := $(CROSS_COMPILE)gcc
CFLAGS := -Wall -Wextra

ifeq ($(CROSS_COMPILE),)
    TARGET := writer
else
    TARGET := writer_cross
endif

SRCS := writer.c
OBJS := $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)


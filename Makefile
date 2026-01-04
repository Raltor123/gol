CC = gcc
CFLAGS = -Iinclude -I/usr/include/pipewire-0.3 -I/usr/include/spa-0.2 -D_REENTRANT -I/usr/include/libdrm -I/usr/include/libdecor-0 -O3 -march=native -fopenmp
LDLIBS = -lSDL3 -lm

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = main

NPROC := $(shell nproc)

$(TARGET): $(OBJS)

bear:
	$(RM) compile_commands.json
	bear -- $(MAKE) -j$(NPROC)

clean:
	$(RM) $(TARGET) $(OBJS)

#Makefile
CC      = gcc
CFLAGS  = -g -Wall -O2
TARGET  = bitmap
SRCS    = bitmap.c
C_OBJS  = bitmap.o

all:$(TARGET)

$(TARGET):$(C_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<
.PHONY:clean
clean:
	rm -rf *.o $(TARGET) $(CXX_OBJS) $(C_OBJS)
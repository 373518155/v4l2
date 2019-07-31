#Makefile
CC      = gcc
CFLAGS  = -g -Wall -O2
TARGET  = v4l2
SRCS    = v4l2.c
C_OBJS  = v4l2.o

all:$(TARGET)

$(TARGET):$(C_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<
.PHONY:clean
clean:
	rm -rf *.o $(TARGET) $(CXX_OBJS) $(C_OBJS)
	rm ./img*
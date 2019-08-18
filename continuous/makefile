#Makefile
CC      = gcc
CFLAGS  = -g -Wall -O2
TARGET  = continuous
SRCS    = continuous.c
C_OBJS  = continuous.o

all:$(TARGET)

$(TARGET):$(C_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<
.PHONY:clean
clean:
	rm -rf *.o $(TARGET) $(CXX_OBJS) $(C_OBJS)
	rm ./img*
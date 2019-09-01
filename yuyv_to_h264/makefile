#Makefile
CC      = gcc
CFLAGS  = -g -Wall -O2 -I/usr/local/include -L/usr/local/lib -lx264
TARGET  = simplest_x264_encoder
SRCS    = simplest_x264_encoder.c
C_OBJS  = simplest_x264_encoder.o

all:$(TARGET)

$(TARGET):$(C_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
%.o:%.c
	$(CC) -c -o $@ $< $(CFLAGS)
.PHONY:clean
clean:
	rm -rf *.o $(TARGET) $(CXX_OBJS) $(C_OBJS)

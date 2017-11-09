CC = arm-none-linux-gnueabi-gcc 
TARGET = main 
SRCS = $(wildcard *.c) 
OBJS = $(patsubst %.c, %.o, $(SRCS))

$(TARGET):$(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -L. -lsqlite3_arm -lpthread -ldl
	rm *.o 

$(OBJS):%.o:%.c
	$(CC) -c $^ -o $@

clean:
	rm *.o ./src/*.o

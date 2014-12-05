CC=gcc
OBJS = MySocket.o MyClient.o
TARGET = MyClient

.SUFFIXES : .o .c

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(objs)

clean : 
	rm -f $(OBJS) $(TARGET)

CC=gcc
CFLAGS=-Wall -ggdb

MessageQueueApp: main.o message_queue.o files.o server.o client.o semaphore.o
	$(CC) $(CFLAGS) main.o message_queue.o files.o server.o client.o semaphore.o -o MessageQueueApp -lpthread

clean:
	rm -f *.o *.bak MessageQueueApp

main.o:
	$(CC) $(CFLAGS) -O -c main.c

message_queue.o:
	$(CC) $(CFLAGS) -O -c message_queue.c

files.o:
	$(CC) $(CFLAGS) -O -c files.c

server.o:
	$(CC) $(CFLAGS) -O -c server.c

client.o:
	$(CC) $(CFLAGS) -O -c client.c

semaphore.o:
	$(CC) $(CFLAGS) -O -c semaphore.c
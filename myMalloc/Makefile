CC = gcc
CFLAGS = -g -std=c99 -Wall -fsanitize=address,undefined

memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) memgrind.o -o $@

memgrind.o: memgrind.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c $^

clean:
	rm *.o memgrind
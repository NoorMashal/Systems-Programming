CC = gcc
CFLAGS = -g -std=c99 -Wall -fsanitize=address,undefined

words: words.o 
	$(CC) $(CFLAGS) words.o -o $@

words.o: words.c 
	$(CC) $(CFLAGS) -c $^

clean:
	rm *.o words
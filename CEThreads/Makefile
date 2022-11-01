CC=gcc  #compiler
CFLAGS=-Wall -Wextra -pedantic -Werror -g -std=gnu11 -Wno-language-extension-token
 
all:	CEthreads.o demo-CEthreads.o
	$(CC) $(LDFLAGS) -c -g CEthreads.c
	$(CC) $(LDFLAGS) -c -g demo-CEthreads.c
	$(CC) $(LDFLAGS) -o demo-CEthreads demo-CEthreads.o CEthreads.o 
clean:
	rm *.o $(TARGET)
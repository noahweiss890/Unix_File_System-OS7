CC = gcc
CFLAGS = -fPIC -Wall -Wextra -O2 -g
SOURCES = my_ufs.c mystdio.c test.c
HEADERS = my
OBJS = $(SOURCES:.c=.o)
LDFLAGS = -shared

all: test

test: test.o libmylibc.so
	$(CC) -o test test.o ./libmylibc.so

libmylibc.so: $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^ -fPIC

test.o: test.c
	$(CC) -c test.c

my_ufs.o: my_ufs.c my_ufs.h
	$(CC) -c my_ufs.c -fPIC

mystdio.o: mystdio.c mystdio.h
	$(CC) -c mystdio.c -fPIC

clean:
	rm -f *.o myFile.txt test

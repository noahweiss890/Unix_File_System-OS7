CC = gcc
CFLAGS = -fPIC -Wall -Wextra -O2 -g
SOURCES = my_ufs.c mystdio.c main.c
HEADERS = my
OBJS = $(SOURCES:.c=.o)
LDFLAGS = -shared

all: main

main: main.o libmylibc.so
	$(CC) -o main main.o ./libmylibc.so

libmylibc.so: $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^ -fPIC

main.o: main.c
	$(CC) -c main.c

my_ufs.o: my_ufs.c my_ufs.h
	$(CC) -c my_ufs.c -fPIC

mystdio.o: mystdio.c mystdio.h
	$(CC) -c mystdio.c -fPIC

clean:
	rm -f *.o myFile.txt main libmylibc.so

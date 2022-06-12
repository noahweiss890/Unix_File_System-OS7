all: test

test: test.o my_ufs.o mystdio.o
	gcc -o test test.o my_ufs.o mystdio.o

test.o: test.c my_ufs.h mystdio.h
	gcc -c test.c

my_ufs.o: my_ufs.c my_ufs.h
	gcc -c my_ufs.c

mystdio.o: mystdio.c mystdio.h
	gcc -c mystdio.c

clean:
	rm -f *.o myFile.txt test

all: test

test: test.o my_ufs.o
	gcc -o test test.o my_ufs.o

test.o: test.c my_ufs.h
	gcc -c test.c

my_ufs.o: my_ufs.c my_ufs.h
	gcc -c my_ufs.c

clean:
	rm *.o myFile.txt test
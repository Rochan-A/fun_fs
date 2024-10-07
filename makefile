all: test

test: test.o fs.o
	gcc -o test test.o fs.o

test.o: test.c fs.h
	gcc -c test.c

fs.o: fs.h fs.c
	gcc -c fs.c
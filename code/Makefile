myshell: main.o parse.o
	gcc -o myshell main.o parse.o
main.o: main.c parse.h
	gcc -c main.c -Wall
parse.o: parse.c parse.h
	gcc -c parse.c -Wall
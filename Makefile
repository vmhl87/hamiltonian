all: run test

run: *.c
	gcc main.c -o run

test:
	./run

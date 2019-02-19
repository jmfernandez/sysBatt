CFLAGS:=-O2 -Wall -g3

sysBatt: sysBatt.o main.o
	gcc -o $@ $^

sysBatt2: sysBatt.o main2.o
	gcc -o $@ $^
	
all: sysBatt sysBatt2
main2.o: main2.c
main.o: main.c
sysBatt.o: sysBatt.c

main.o: main.c
	gcc -o main.o -c main.c

trab2: main.o
	gcc -o trab2 main.o -lm

all: trab2

run: all
	./trab2

clean: all
	rm -rf trab2 *.o

zip: clean
	zip -r trab_1.zip ./*

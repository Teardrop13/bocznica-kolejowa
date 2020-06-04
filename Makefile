all: build run

build:
	g++ -o bocznica bocznica.cpp -lncurses

run:
	./bocznica

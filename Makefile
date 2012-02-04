.PHONY: all test clean
CC=gcc
CFLAGS=-Wall
OBJECTS=main.o msh.o

all: $(OBJECTS) test
	$(CC) $(CFLAGS) -o msh $(OBJECTS)

point4:
	$(CC) $(CFLAGS) -o point4 src/point4.c

test: $(OBJECTS)
	$(CC) $(CFLAGS) -o test_msh msh.o test/test.c -I"src/"
	./test_msh

main.o:
	$(CC) $(CFLAGS) -c src/main.c

msh.o:
	$(CC) $(CFLAGS) -c src/msh.c

clean:
	rm $(OBJECTS) msh test_msh

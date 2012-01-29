all:
	gcc -Wall -o msh src/main.c src/msh.h src/msh.c

clean:
	rm msh

.PHONY: all

all: dynamic static

dynamic: 
	gcc -c main.c
	gcc -o dynamic main.o

static:
	gcc -c main.c
	gcc --static -o static main.o -lc_nonshared

clean:
	rm dynamic static main.o 2>/dev/null ; true

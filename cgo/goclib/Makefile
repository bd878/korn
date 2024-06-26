a.out : main.c bin/shared/libgoc.so
	gcc main.c -Lbin/shared/ -lgoc -o a.out

bin/shared/libgoc.so : goc.o 
	gcc -shared goc.o -o bin/shared/libgoc.so

goc.o : goc.c goc.h
	gcc -c -fPIC goc.c

clean :
	rm a.out goc.o bin/shared/libgoc.so

obj-m += sleep.o

cat_nonblock : cat_nonblock.c
	gcc cat_nonblock.c -o cat_nonblock

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) -k modules

clean:
	make -C /lib/modules/`uname -r`/build M=$(PWD) build

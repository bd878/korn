obj-m += hello.o
obj-m += hello-3.o
obj-m += hello-5.o

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) -k modules

clean:
	make -C /lib/modules/`uname -r`/build M=$(PWD) clean

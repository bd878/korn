obj-m += devicemodel.o

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) -k modules

clean:
	make -C /lib/modules/`uname -r`/build M=$(PWD) clean 

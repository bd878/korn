obj-m += procfs1.o
obj-m += procfs2.o
obj-m += procfs3.o

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) -k modules

clean:
	make -C /lib/modules/`uname -r`/build M=$(PWD) clean 

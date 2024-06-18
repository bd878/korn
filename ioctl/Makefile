obj-m += ioctl.o
obj-m += chardev2.o

userspace_ioctl.c : userspace_ioctl.out
	gcc userspace_ioctl.c -o userspace_ioctl.out

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) -k modules

clean:
	make -C /lib/modules/`uname -r`/build M=$(PWD) clean 

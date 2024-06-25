obj-m += example_mutex.o
obj-m += example_spinlock.o
obj-m += example_rwlock.o
obj-m += example_atomic.o

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) -k modules

clean:
	make -C /lib/modules/`uname -r`/build M=$(PWD) clean

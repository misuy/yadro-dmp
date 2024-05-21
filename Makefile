obj-m = dmp.o
PWD = $(shell pwd)

.PHONY: test clean

all: build

build:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules

dmp.ko: build

insmod: dmp.ko
	insmod dmp.ko

rmmod:
	rmmod dmp

clean:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean
	rm -rf tests
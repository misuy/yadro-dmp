obj-m = dmp.o
dmp-objs += src/dev_stat.o src/dmp.o
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

test:
	bash test.bash

clean:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean

#!/bin/bash
losetup -d /dev/loop6
dmsetup remove dmp1
dmsetup remove dmp2

dd if=/dev/zero of=/tmp/disk2048 bs=512 count=4
losetup --sector-size 512 /dev/loop6 /tmp/disk2048

dmsetup create dmp1 --table "0 4 dmp /dev/loop6 dmp1"

dd if=/dev/random of=/dev/mapper/dmp1 bs=512 count=1

dd if=/dev/random of=/dev/mapper/dmp1 bs=512 count=2

dd if=/dev/random of=/dev/mapper/dmp1 bs=512 count=4

cat /sys/module/dmp/devices/dmp1/stat
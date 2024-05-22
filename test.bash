#!/bin/bash


# size in 512-byte sectors
for ((size=1; size <= 256; size=size*2))
do
    dmsetup remove dmp$size
    losetup -d /dev/loop6
    rm /tmp/disk$size

    echo
    echo "creating file /tmp/disk$size (size $((512 * $size)))"
    dd if=/dev/zero of=/tmp/disk$size bs=512 count=$size

    echo
    echo "attaching /dev/loop6 to /tmp/disk$size"
    losetup --sector-size 512 /dev/loop6 /tmp/disk$size

    echo
    echo "creating dmp$size over /dev/loop6"
    dmsetup create dmp$size --table "0 $size dmp /dev/loop6 dmp$size"

    for ((i=0; i < 10; i++))
    do
        dd if=/dev/random of=/dev/mapper/dmp$size bs=512 count=$size
    done

    echo
    echo "--------------------DMP$size STAT--------------------"
    cat /sys/module/dmp/devices/dmp$size/stat
    echo "-------------------------------------------------"        
    echo

    echo "cleaning up"
    dmsetup remove dmp$size
    losetup -d /dev/loop6
    rm /tmp/disk$size
    echo
    echo
    echo
    echo
done

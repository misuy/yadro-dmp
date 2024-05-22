## YADRO-DMP (device mapper proxy)

DMP -- Linux device mapper target, that processes I/O requests to an underlying devices, and collect data about count of requests and average size of transmitted blocks. This statistics can be accessed through sysfs.

#### Комментарий
Релизация соответствует описанию в ТЗ, за исключением одной детали: \
Судя по всему, в задании подразумевалось, что модуль накапливает статистические данные в одном, общем для всех устройств хранилище, т.к. это хранилище ассоциировано с самим модулем (cat /sys/module/dmp/stat/volumes). \
Мне показалось, что было бы здорово хранить статистические данные для каждого устройства отдельно. В связи с этим получившийся интерфейс взаимодействия с модулем немного отличается от интерфейса, описанного в задании. Он описан в user-guide.

### user-guide
#### build
```shell
make build
```

#### install
```shell
make insmod
```

#### create new virt device
```shell
dmsetup create <device_name> --table "0 <size_in_sectors> dmp <underlying/device/path> <name_for_stat>"

# created device path: /dev/mapper/<device_name>
```

#### get device stat
```shell
cat /sys/module/dmp/devices/<name_for_stat>/stat

# read:
#   reqs: %u (read requests count)
#   avg_size: %u (average size of blocks to read)
# write:
#   reqs: %u (write requests count)
#   avg_size: %u (average size of blocks to write)
# total:
#   reqs: %u (total requests count)
#   avg_size: %u (average size of all blocks)
```

#### test
```shell
make test

# dmpN -- means N 512-byte sectors in underlying device
# ...
# ...
# --------------------DMP1 STAT--------------------
# read:
#   reqs: 5
#   avg_size: 512
# write:
#   reqs: 10
#   avg_size: 512
# total:
#   reqs: 15
#   avg_size: 512
# -------------------------------------------------
# ...
# ...
# ...
# --------------------DMP2 STAT--------------------
# read:
#   reqs: 13
#   avg_size: 1024
# write:
#   reqs: 10
#   avg_size: 1024
# total:
#   reqs: 23
#   avg_size: 1024
# -------------------------------------------------
# ...
# ...

```

#### remove device
```shell
dmsetup remove <device_name>
```

#### remove module
```shell
make rmmod
```

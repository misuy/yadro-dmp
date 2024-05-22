## YADRO-DMP (device mapper proxy)

#### Комментарий
Релизация соответствует описанию в ТЗ, за исключением одной детали: \
Судя по всему, в задании подразумевалось, что модуль накапливает статистические данные в одном хранилище, общем для всех устройств, т.к. это хранилище ассоциировано с самим модулем (cat /sys/module/dmp/stat/volumes). \
Мне показалось, что было бы здорово хранить статистические данные для каждого устройства отдельно. В связи с этим получившийся интерфейс взаимодействия с модулем немного отличается от интерфейса, описанного в задании. Он будет рассмотрен далее.

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
dmsetup create <device_name> --table "0 <size> dmp <underlying/device/path> <name_for_stat>"
```

#### get device stat
```shell
cat /sys/module/dmp/devices/<name_for_stat>/stat

# read:
#   reqs: %u
#   avg_size: %u
# write:
#   reqs: %u
#   avg_size: %u
# total:
#   reqs: %u
#   avg_size: %u
```

#### test
```shell
make test

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

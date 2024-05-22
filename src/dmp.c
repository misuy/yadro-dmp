#include <linux/device-mapper.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/bio.h>

#include "dev_stat.h"


extern struct kobj_type dev_stat_type;

#define DM_MSG_PREFIX "dmp"

struct dmp_target {
	struct dm_dev *dev;
	struct dev_stat *dev_stat;
};

#define KSET_DEVICES_NAME "devices"
static struct kset *devices;


static int dmp_ctr(struct dm_target *ti, unsigned int argc, char **argv)
{
	struct dmp_target *target;

	if (argc != 2) {
		ti->error = "invalid arguments count";
		return -EINVAL;
	}

	target = kmalloc(sizeof(struct dmp_target), GFP_KERNEL);
	if (!target) {
		ti->error = "dmp_target memory allocation error";
		return -ENOMEM;
	}

	target->dev_stat = kmalloc(sizeof(struct dev_stat), GFP_KERNEL);
	if (!target->dev_stat) {
		ti->error = "dev_stat memory allocation error";
		return -ENOMEM;
	}

	if (dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &target->dev)) {
		ti->error = "device is not opened correctly";
		goto dmp_ctr_fail;
	}

	target->dev_stat->kobj.kset = devices;

	if (kobject_init_and_add(&target->dev_stat->kobj, &dev_stat_type, NULL, argv[1])) {
		ti->error = "dev_stat kobject creation error";
		kobject_put(&target->dev_stat->kobj);
		goto dmp_ctr_fail;
	}

	kobject_uevent(&target->dev_stat->kobj, KOBJ_ADD);

	target->dev_stat->read_req_cnt = 0;
	target->dev_stat->write_req_cnt = 0;
	target->dev_stat->read_avg_blk_sz = 0;
	target->dev_stat->write_avg_blk_sz = 0;

	ti->private = target;

	return 0;

dmp_ctr_fail:
	kfree(target);
	return -EINVAL;
}


static void dmp_dtr(struct dm_target *ti) {
	struct dmp_target *target = (struct dmp_target *) ti->private;

	printk(KERN_CRIT ">> dmp_dtr");
	dm_put_device(ti, target->dev);
	kobject_put(&target->dev_stat->kobj);
	kfree(target);
}


static int dmp_map(struct dm_target *ti, struct bio *bio)
{
	struct dmp_target *target = (struct dmp_target *) ti->private;
	dev_stat_add_req(target->dev_stat, bio);

	bio_set_dev(bio, target->dev->bdev);
	submit_bio(bio);

	return DM_MAPIO_SUBMITTED;
}


static struct target_type dmp_target = {
	.name    = "dmp",
	.version = {0, 0, 0},
	.module  = THIS_MODULE,
	.ctr     = dmp_ctr,
	.dtr     = dmp_dtr,
	.map     = dmp_map,
};



static int __init init_dmp(void) {
	if (dm_register_target(&dmp_target) < 0) {
		printk(KERN_CRIT "dm target registration error");
		return -EINVAL;
	}

	devices = kset_create_and_add(KSET_DEVICES_NAME, NULL, &THIS_MODULE->mkobj.kobj);
	if (!devices) {
		printk(KERN_CRIT "deivices kset init error");
		return -EINVAL;
	}

	return 0;
}


static void __exit exit_dmp(void) {
	dm_unregister_target(&dmp_target);
	kset_unregister(devices);
}


module_init(init_dmp);
module_exit(exit_dmp);

MODULE_AUTHOR("Mihail Peredriy <mperedriy@bk.ru>");
MODULE_DESCRIPTION(DM_NAME " ...");
MODULE_LICENSE("GPL");

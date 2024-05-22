#include <linux/device-mapper.h>

#include <linux/module.h>
#include <linux/init.h>
#include <linux/bio.h>

#include "fixed_point_arithmetic.h"


#define DM_MSG_PREFIX "dmp"

struct dmp_target {
	struct dm_dev *dev;
	struct dmp_dev_stat dev_stat;
}


struct dmp_dev_stat {
	struct kobject kobj;

	u32 read_req_cnt;
	u32 write_req_cnt;
	uf64 read_avg_blk_sz;
	uf64 write_avg_blk_sz;
}

#define ATTR_STAT_NAME "stat"

struct attribute stat_attr = (struct attribute) { ATTR_STAT_NAME, S_IRUGO };

ssize_t dev_stat_show(struct kobject *kobj, struct attribute *attr, char *buffer) {
	if (attr != &stat_attr)
		return 0;

	struct dmp_dev_stat *stat = container_of(kobj, struct dmp_dev_stat, kobj);

	struct uf64 total_avg_blk_sz =
		UF64_DIV(
			UF64_ADD(
				UF64_MUL(
					stat->read_avg_blk_sz, 
					UF64_FROM_U32(stat->read_req_cnt)
				),
				UF64_MUL(
					stat->write_avg_blk_sz, 
					UF64_FROM_U32(stat->write_req_cnt)
				)
			),
			UF64_ADD(
				UF64_FROM_U32(stat->read_req_cnt),
				UF64_FROM_U32(stat->write_req_cnt)
			)
		);

	return
		sysfs_emit(
			buffer,
			"read:\n"
			"  reqs: %ul\n"
			"  avg_size: %ul.%ul\n"
			"write:\n"
			"  reqs: %ul\n"
			"  avg_size: %ul.%ul\n"
			"total:"
			"  reqs: %ul\n"
			"  avg_size: %ul.%ul\n",
			stat->read_req_cnt,
			UF64_GET_INT_PART(stat->read_avg_blk_sz), UF64_GET_FRAC_PART(stat->read_avg_blk_sz, 7),
			stat->write_req_cnt,
			UF64_GET_INT_PART(stat->write_avg_blk_sz), UF64_GET_FRAC_PART(stat->write_avg_blk_sz, 7),
			stat->read_req_cnt + stat->write_req_cnt,
			UF64_GET_INT_PART(total_avg_blk_sz), UF64_GET_FRAC_PART(total_avg_blk_sz, 7)
		);
}
struct sysfs_ops dev_stat_ops = (struct sysfs_ops) { stat_show, NULL };

void dev_stat_release(struct kobject *kobj) {
	struct dmp_dev_stat *stat = container_of(kobj, struct dmp_dev_stat, kobj);
	kfree(stat);
}

struct kobj_type dev_stat_type = (struct kobj_type) { dev_stat_release, dev_stat_ops };


#define KSET_DEVICES_NAME "devices"
struct kset *devices;



static int dmp_ctr(struct dm_target *ti, unsigned int argc, char **argv)
{
	if (argc != 1) {
		ti->error = "invalid arguments count";
		return -EINVAL;
	}

	struct dmp_target *target = kmalloc(sizeof(struct dmp_target), GFP_KERNEL);
	if (!ti->private) {
		ti->error = "dmp_target memory allocation error";
		return -ENOMEM;
	}

	if (dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &target->dev)) {
		ti->error = "device is not opened correctly";
		goto dmp_ctr_fail;
	}

	if (dm)

	target->dev

	return 0;

dmp_ctr_fail:
	free(ti->private);
	return -EINVAL;
}

/*
 * Return zeros only on reads
 */
static int zero_map(struct dm_target *ti, struct bio *bio)
{
	switch (bio_op(bio)) {
	case REQ_OP_READ:
		if (bio->bi_opf & REQ_RAHEAD) {
			/* readahead of null bytes only wastes buffer cache */
			return DM_MAPIO_KILL;
		}
		zero_fill_bio(bio);
		break;
	case REQ_OP_WRITE:
	case REQ_OP_DISCARD:
		/* writes get silently dropped */
		break;
	default:
		return DM_MAPIO_KILL;
	}

	bio_endio(bio);

	/* accepted bio, don't make new request */
	return DM_MAPIO_SUBMITTED;
}

static void zero_io_hints(struct dm_target *ti, struct queue_limits *limits)
{
	limits->max_discard_sectors = UINT_MAX;
	limits->max_hw_discard_sectors = UINT_MAX;
	limits->discard_granularity = 512;
}

static struct target_type zero_target = {
	.name   = "zero",
	.version = {1, 2, 0},
	.features = DM_TARGET_NOWAIT,
	.module = THIS_MODULE,
	.ctr    = zero_ctr,
	.map    = zero_map,
	.io_hints = zero_io_hints,
};
module_dm(zero);

MODULE_AUTHOR("Mihail Peredriy <mperedriy@bk.ru>");
MODULE_DESCRIPTION(DM_NAME " ...");
MODULE_LICENSE("GPL");
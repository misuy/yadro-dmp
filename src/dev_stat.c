#include "dev_stat.h"

void dev_stat_add_req(struct dev_stat *stat, struct bio *req) {
	switch (bio_op(req)) {
		case REQ_OP_READ:
			stat->read_avg_blk_sz = 
				(stat->read_req_cnt * stat->read_avg_blk_sz + req->bi_iter.bi_size) / 
				(stat->read_req_cnt + 1);
			stat->read_req_cnt++;
			break;
		case REQ_OP_WRITE:
			stat->write_avg_blk_sz = 
				(stat->write_req_cnt * stat->write_avg_blk_sz + req->bi_iter.bi_size) / 
				(stat->write_req_cnt + 1);
			stat->write_req_cnt++;
			break;
		default:
	}
}

static ssize_t stat_show(struct dev_stat *stat, struct stat_attribute *attr, char *buf) {
		u32 total_avg_blk_sz = 
			( stat->read_req_cnt * stat->read_avg_blk_sz +
				stat->write_req_cnt * stat->write_avg_blk_sz) / 
			(stat->read_req_cnt + stat->write_req_cnt);

	return sysfs_emit(
		buf,
		"read:\n"
		"  reqs: %u\n"
		"  avg_size: %u\n"
		"write:\n"
		"  reqs: %u\n"
		"  avg_size: %u\n"
		"total:\n"
		"  reqs: %u\n"
		"  avg_size: %u\n",
		stat->read_req_cnt,
		stat->read_avg_blk_sz,
		stat->write_req_cnt,
		stat->write_avg_blk_sz,
		stat->read_req_cnt + stat->write_req_cnt,
		total_avg_blk_sz
	);
}

static struct stat_attribute stat_attr = __ATTR(stat, S_IRUGO, stat_show, NULL);

static struct attribute *dev_stat_default_attrs[] = {
	&stat_attr.attr,
	NULL
};
ATTRIBUTE_GROUPS(dev_stat_default);



static ssize_t dev_stat_attr_show(struct kobject *kobj, struct attribute *attr, char *buffer) {
	struct dev_stat *stat = container_of(kobj, struct dev_stat, kobj);
	struct stat_attribute *stat_attr = container_of(attr, struct stat_attribute, attr);

	if (!stat_attr->show) {
		return -EIO;
	}

	return stat_attr->show(stat, stat_attr, buffer);
}

static struct sysfs_ops dev_stat_sysfs_ops = (struct sysfs_ops) { 
	.show = dev_stat_attr_show
};

static void dev_stat_release(struct kobject *kobj) {
	struct dev_stat *stat;
	stat = container_of(kobj, struct dev_stat, kobj);
	kfree(stat);
}

struct kobj_type dev_stat_type = (struct kobj_type) {
	.release = dev_stat_release,
	.sysfs_ops = &dev_stat_sysfs_ops,
	.default_groups = dev_stat_default_groups
};

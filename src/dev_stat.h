#ifndef _DEV_STAT
#define _DEV_STAT

#include <linux/device-mapper.h>


struct dev_stat {
	struct kobject kobj;

	u32 read_req_cnt;
	u32 write_req_cnt;
	u32 read_avg_blk_sz;
	u32 write_avg_blk_sz;
};

#define ATTR_STAT_NAME "stat"

struct stat_attribute {
	struct attribute attr;
	ssize_t (*show) (struct dev_stat *stat, struct stat_attribute *attr, char *buf);
	ssize_t (*store)(struct dev_stat *stat, struct stat_attribute *attr, const char *buf, size_t count);
};


void dev_stat_add_req(struct dev_stat *stat, struct bio *req);


#endif

#ifndef LINUX_IO_RQ_COUNTER_H
#define LINUX_IO_RQ_COUNTER_H

#define NETLINK_USER 31
#define MAX_PAYLOAD 32

struct km_request {
	int code;
	union {
		char buf[MAX_PAYLOAD];
		int val;
	} data;
};

void init_counter_syscalls(void);
int mount_counter(char *dev);
int get_num_io_rqs(char *dev);

#endif

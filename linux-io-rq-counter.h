
#ifndef LINUX_IO_REQUEST_COUNTER_H
#define LINUX_IO_REQUEST_COUNTER_H

struct km_request {
	int code;
	union {
		void *buf;
		int val;
	} data;
};

void init_counter_syscalls(void);
int mount_counter(char *dev);
int get_num_io_rqs(char *dev);

#endif

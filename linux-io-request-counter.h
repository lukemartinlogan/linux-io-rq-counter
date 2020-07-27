
#ifndef LINUX_IO_REQUEST_COUNTER_H
#define LINUX_IO_REQUEST_COUNTER_H

static struct km_request {
	int code;
	union {
		void *buf;
		int val;
	} data;
};

#endif

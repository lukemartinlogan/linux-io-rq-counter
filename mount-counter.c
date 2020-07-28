
#include <stdio.h>
#include <stdlib.h>
#include <linux-io-rq-counter.h>

int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("USAGE: ./mount-counter [path/to/device/file]\n");
		exit(1);
	}
	char *dev = argv[1];
	int code = 0;
	
	printf("Mounting %s\n", dev);
	init_counter_syscalls();
	code = mount_counter(dev);
	printf("CODE: %d\n", code);
}

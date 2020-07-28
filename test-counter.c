
#include <stdio.h>
#include <stdlib.h>
#include <linux-io-rq-counter.h>

int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("USAGE: ./test-counter [path/to/device/file]\n");
		exit(1);
	}
	char *dev = argv[1];
	int num_rqs = 0;
	
	init_counter_syscalls();
	num_rqs = get_num_io_rqs(dev);
	printf("There were %d io requests pending or currently being processed by %s\n", num_rqs, dev);
}

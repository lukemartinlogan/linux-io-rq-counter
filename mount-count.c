
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux-io-request-counter.h>

static int sockfd;
static struct sockaddr_nl my_addr = {0};
static struct sockaddr_nl kern_addr = {0};

void init_mount_syscall(void)
{
	sockfd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_USER);
	
	my_addr.nl_family = AF_NETLINK;
	my_addr.nl_pad = 0;
	my_addr.nl_pid = getpid();
	my_addr.nl_groups = 0;
	
	kern_addr.nl_family = AF_NETLINK;
	kern_addr.nl_pad = 0;
	kern_addr.nl_pid = 0;
	kern_addr.nl_groups = 0;
}

int mount_count(void)
{
	int num_io_reqs = 0;
	
	struct nlmsghdr nl_header = {
	};
	
	int ret = sendto(sockfd, (void*)nl_header, nl_header->nlmsg_len, 0, (struct sockaddr *)&kern_addr, sizeof(struct sockaddr_nl));
	if(ret < 0) {
		perror("Unable to send message to kernel module\n");
		return -1;
	}
	
	int ret = recvfrom(sockfd, (void*)&num_io_reqs, sizeof(int), 0, &kern_addr, sizeof(struct sockaddr_nl));
	if(ret < 0) {
		perror("Unable to recv count from kernel module\n");
		return -1;
	}
	
	return num_io_reqs;
}

int main(void)
{
	init_mount_syscall();
}

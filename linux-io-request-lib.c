
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux-io-request-counter.h>

#define MAX_PAYLOAD 1024

//Global vars
static int sockfd;
static struct sockaddr_nl my_addr = {0};
static struct sockaddr_nl kern_addr = {0};

void init_counter_syscall(void)
{
	sockfd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_USER);
	
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.nl_family = AF_NETLINK;
	my_addr.nl_pid = getpid();
	
	memset(&kern_addr, 0, sizeof(kern_addr));
	kern_addr.nl_family = AF_NETLINK;
	kern_addr.nl_pid = 0;
	
	bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
}

int get_num_io_requests(char *dev)
{
	int num_io_reqs = 0;
	struct nlmsghdr *nlh;
	struct km_request *rq;
	
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_flags = 0;
	rq = NLMSG_DATA(nlh);
	
	rq->code = 2;
	rq->data.buf = dev;
	
	int ret = sendto(sockfd, (void*)nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&kern_addr, sizeof(struct sockaddr_nl));
	if(ret < 0) {
		perror("Unable to send message to kernel module\n");
		return -1;
	}
	
	int ret = recvfrom(sockfd, (void*)nlh, sizeof(int), 0, &kern_addr, sizeof(struct sockaddr_nl));
	if(ret < 0) {
		perror("Unable to recv count from kernel module\n");
		return -1;
	}
	
	rq = NLMSG_DATA(nlh);
	num_io_reqs = rq->data.val;
	
	free(nlh);
	return num_io_reqs;
}



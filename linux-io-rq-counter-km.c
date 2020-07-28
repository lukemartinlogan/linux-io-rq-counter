
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/blk-mq.h>
#include <linux/netlink.h>
#include <linux/connector.h>

MODULE_AUTHOR("Luke Logan <llogan@hawk.iit.edu>");
MODULE_DESCRIPTION("A simple test to acquire the current number of queued and issued requeusts for a particular device");
MODULE_LICENSE("GPL");
MODULE_ALIAS_FS("linux-io-rq-counter-km");

//Macros
#define KERNEL_VERSION_4
#define NETLINK_USER 31
#define MAX_PAYLOAD 32
#define MAX_MOUNTED_BDEVS 32

//Data definitions
struct dev_data {
	int is_active;
	char name[MAX_PAYLOAD];
	struct block_device *bdev;
};
struct km_request {
	int code;
	union {
		char buf[MAX_PAYLOAD];
		int val;
	} data;
};
static int queue_tail = 0;
static struct dev_data device_list[MAX_MOUNTED_BDEVS] = {0};
struct sock *nl_sk = NULL;

//Prototypes
static int __init init_io_request_counter(void);
static int start_server(void);
static void server_loop(struct sk_buff *skb);
static struct dev_data *alloc_block_device(char *dev);
static struct dev_data *find_block_device(char *dev);
static void mount_device(char *dev, int pid);
static void get_num_io_requests(char *dev, int pid);
static void __exit exit_io_request_counter(void);

//Implementation
static int __init init_io_request_counter(void)
{
	printk(KERN_INFO "linux_io_rq_counter_km: Initializing module");
	return start_server();
}

static int start_server(void)
{
	struct netlink_kernel_cfg cfg = {
		.input = server_loop,
	};

	nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
	if(!nl_sk)
	{
		printk(KERN_ALERT "linux_io_rq_counter_km: Error creating socket.\n");
		return -10;
	}
	printk(KERN_INFO "linux_io_rq_counter_km: Netlink socket initialized");
	return 0;
}

static void server_loop(struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	struct km_request *rq;
	int pid;

	printk(KERN_INFO "linux_io_rq_counter_km: Entering: %s\n", __FUNCTION__);
	
	nlh=(struct nlmsghdr*)skb->data;
	rq = (struct km_request*)nlmsg_data(nlh);
	pid = nlh->nlmsg_pid; /*pid of sending process */
	printk(KERN_INFO "linux_io_rq_counter_km: Code %d", rq->code);
	
	switch(rq->code) {
		case 1: {
			mount_device(rq->data.buf, pid);
			break;
		}
		
		case 2: {
			get_num_io_requests(rq->data.buf, pid);
			break;
		}
	}
}

static struct dev_data *alloc_block_device(char *dev)
{
	struct dev_data *dd;
	int i = 0;
	
	for(i = 0; i < MAX_MOUNTED_BDEVS; i++) {
		dd = device_list + (queue_tail + i)%32;
		if(!dd->is_active) {
			dd->is_active = 1;
			strcpy(dd->name, dev);
			queue_tail = (queue_tail + 1)%32;
			return dd;
		}
	}
	
	return NULL;
}

static struct dev_data *find_block_device(char *dev)
{
	int i = 0;
	
	for(i = 0; i < MAX_MOUNTED_BDEVS; i++) {
		if(strcmp(dev, device_list[i].name) == 0) {
			return device_list + i;
		}
	}
	
	return NULL;
}

static struct dev_data *release_all_block_devices(void)
{
	struct dev_data *dd;
	int i = 0;
	
	for(i = 0; i < MAX_MOUNTED_BDEVS; i++) {
		dd = device_list + i;
		if(dd->is_active) {
			blkdev_put(dd->bdev, 0);
		}
	}
	
	return NULL;
}

static void send_msg_to_usr(int code, int val, int pid)
{
	struct nlmsghdr *nlh;
	struct sk_buff *skb_out;
	int res = 0;
	struct km_request *rq;
	
	skb_out = nlmsg_new(sizeof(struct km_request), 0);
	if(!skb_out) {
		printk(KERN_ERR "linux_io_rq_counter_km: Failed to allocate new skb\n");
		return;
	} 
	nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, sizeof(struct km_request), 0);
	NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
	rq = nlmsg_data(nlh);
	rq->code = code;
	rq->data.val = val;
	res=nlmsg_unicast(nl_sk, skb_out, pid);
	if(res<0) {
		printk(KERN_ERR "linux_io_rq_counter_km: Error while sending back to user\n");
	}
}

static void mount_device(char *dev, int pid)
{
	//Acquire a free block device structure
	struct dev_data *dd = alloc_block_device(dev);
	
    //Acquire block device structure
    dd->bdev = blkdev_get_by_path(dev, 0, dd);
    if (IS_ERR(dd->bdev)) {
        printk(KERN_INFO "linux_io_rq_counter_km: can't open bdev <%lu>\n", PTR_ERR(dd->bdev));
        send_msg_to_usr(-1, 0, pid);
        return;
    }
    dd->is_active = 1;
    printk(KERN_INFO "linux_io_rq_counter_km: %s is mounted!\n", dev);
    
    //Send return code back to user
    send_msg_to_usr(0, 0, pid);
    return;
}

static void get_num_io_requests(char *dev, int pid)
{
    struct request_queue *q;
	struct blk_mq_hw_ctx *hctx;
	struct dev_data *dd;
	int total_rqs = 0;
	int i = 0;
	
	//Find block device
	dd = find_block_device(dev);
	if(dd == NULL) {
		printk(KERN_INFO "linux_io_rq_counter_km: Could not find block device %s\n", dev);
		send_msg_to_usr(-1, 0, pid);
		return;
	}
	
    //Get request queue for block device
    q = dd->bdev->bd_queue;

    //Compute the number of IO requests for device
	for(i = 0; i < q->nr_hw_queues; ++i) {
		hctx = q->queue_hw_ctx[i];
		total_rqs += hctx->queued; //hctx-run
	}
	
	//Send back to user
	send_msg_to_usr(0, total_rqs, pid);
}

static void __exit exit_io_request_counter(void)
{
	release_all_block_devices();
    netlink_kernel_release(nl_sk);
    printk(KERN_INFO "linux_io_rq_counter_km: Module has been removed!\n");
}

module_init(init_io_request_counter)
module_exit(exit_io_request_counter)

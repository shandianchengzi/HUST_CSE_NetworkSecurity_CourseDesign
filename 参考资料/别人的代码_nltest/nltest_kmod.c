#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <linux/netlink.h>

#define NETLINK_TEST	17

static struct sock *nlsk = NULL;

int nltest_ksend(char *info, int pid)
{
	char reply[256];
	int rlen;
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	int retval;

	sprintf(reply, "NLTEST Reply for '%s'", info);
	rlen = strlen(reply) + 1;

	skb = nlmsg_new(rlen, GFP_ATOMIC);
	if (skb == NULL) {
		printk("alloc reply nlmsg skb failed!\n");
		return -1;
	}

	nlh = nlmsg_put(skb, 0, 0, 0, NLMSG_SPACE(rlen) - NLMSG_HDRLEN, 0);
	memcpy(NLMSG_DATA(nlh), reply, rlen);
	printk("[kernel space] nlmsglen = %d\n", nlh->nlmsg_len);

	//NETLINK_CB(skb).pid = 0;
	NETLINK_CB(skb).dst_group = 0;

	printk("[kernel space] skb->data send to user: '%s'\n", (char *) NLMSG_DATA(nlh));

	retval = netlink_unicast(nlsk, skb, pid, MSG_DONTWAIT);
	printk("[kernel space] netlink_unicast return: %d\n", retval);
	return 0;
}

void nltest_krecv(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = NULL;
	char *data;
	int pid;

	nlh = nlmsg_hdr(skb);
	if ((nlh->nlmsg_len < NLMSG_HDRLEN) || (skb->len < nlh->nlmsg_len)) {
		printk("Illegal netlink packet!\n");
		return;
	}

	data = (char *) NLMSG_DATA(nlh);
	printk("[kernel space] data receive from user: '%s'\n", data);
	pid = nlh->nlmsg_pid;
	printk("[kernel space] user_pid = %d\n", pid);
	nltest_ksend(data, pid);
}

struct netlink_kernel_cfg nltest_cfg = {
	0,	//groups
	0,	//flags
	nltest_krecv,	//input
	NULL,	//cb_mutex
	NULL,	//bind
	NULL,	//unbind
	NULL,	//compare
};

int __init nltest_init(void)
{
	printk("Netlink test module initializing...\n");
	nlsk = netlink_kernel_create(&init_net, NETLINK_TEST, &nltest_cfg);
	if (!nlsk) {
		printk("can not create a netlink socket\n");
		return -1;
	}
	printk("netlink_kernel_create() success, nlsk = %p\n", nlsk);
	return 0;
}

void __exit nltest_exit(void)
{
	sock_release(nlsk->sk_socket);
	printk("Netlink test module exit!\n");
}

module_init(nltest_init);
module_exit(nltest_exit);
MODULE_LICENSE("GPL");

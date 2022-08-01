#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wmz");

static struct nf_hook_ops nfho;

unsigned int telnet_filter(void *priv,
			struct sk_buff *skb,
			const struct nf_hook_state *state)
{
	struct iphdr *iph;
	struct tcphdr *tcph;

	iph = ip_hdr(skb);
	tcph = (void *)iph + iph->ihl*4;

	if(iph->protocol == IPPROTO_TCP && tcph->dest == htons(23))
	{
		printk(KERN_INFO "Dropping telnet packet to %d.%d.%d.%d\n",
		((unsigned char *)&iph->daddr)[0],
		((unsigned char *)&iph->daddr)[1],
		((unsigned char *)&iph->daddr)[2],
		((unsigned char *)&iph->daddr)[3]);
		return NF_DROP;
	}
	else{
		return NF_ACCEPT;
	}
}

static int myfw_init(void)
{
	printk("my firewall module loaded.\n");

	nfho.hook = telnet_filter;
	nfho.pf = PF_INET;
	nfho.hooknum = NF_INET_LOCAL_OUT;
	nfho.priority = NF_IP_PRI_FIRST; //new version, maybe changed to NF_INET_PRI_FIRST

	nf_register_hook(&nfho);
	return 0;
}

static void myfw_exit(void)
{
	printk("my firewall module exit ...\n");
	nf_unregister_hook(&nfho);
}

module_init(myfw_init);
module_exit(myfw_exit);

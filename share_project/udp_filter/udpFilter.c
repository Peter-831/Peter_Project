#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <linux/vmalloc.h>
#include <linux/inet.h>

// Define some structs that will be used
static struct nf_hook_ops nfho;
struct udphdr *udp_header;
struct iphdr *ip_header;
struct ethhdr *mac_header;
struct tcphdr *tcp_header;
const int MAX_NUM = 65536;


// 将两个8bit合为一个16bit
int b8tob16(unsigned char c1, unsigned char c2)
{
    unsigned char t1;
    unsigned char t2;
    t1 = c1;
    t2 = c2;

    return (t1<<8)|t2;
}

// Exchange the first 8bit and the last 8bit between 16bit
int f8tol8(int i)
{
    unsigned char t1;
    unsigned char t2;
    // the last 8bit
    t1 = i&255;
    // the first 8bit
    t2 = (i&65280)>>8;

    return (t1<<8)|t2;
}

// 计算udp校验和
int checkSum(struct iphdr *iphr, struct udphdr *udphr)
{

    struct iphdr *ih = NULL;
    struct udphdr *uh = NULL;
    unsigned char *ip_fields = NULL;
    unsigned char *udp_fields = NULL;
    char *udp_data = NULL;
    int *ud = NULL;
    int *sumarr = NULL;
    int sumcnt = 0;
    int sum = 0;

    ih = iphr;
    uh = udphr;
    ip_fields = (unsigned char *)ih;
    udp_fields = (unsigned char *)uh;
    ud = (int *)vmalloc(100*sizeof(int));
    if(!ud)
    {
        printk("malloc for ud failure!");
        return -1;
    }
    sumarr = (int *)vmalloc(200*sizeof(int));
    if(!sumarr)
    {
        printk("malloc for sumarr failure!");
        return -1;
    }
    
    // 将两个ip地址转变格式
    //int ip_src1 = (ip_fields[12]<<8)|ip_fields[13];
    int ip_src1 = b8tob16(ip_fields[12], ip_fields[13]);
    printk("ip_src[0-1] = 0x%02x%02x\n", ip_fields[12], ip_fields[13]);
    sumarr[sumcnt++] = ip_src1;
    
    int ip_src2 = b8tob16(ip_fields[14], ip_fields[15]); 
    sumarr[sumcnt++] = ip_src2;

    int ip_dest1 = b8tob16(ip_fields[16], ip_fields[17]);
    sumarr[sumcnt++] = ip_dest1;
    
    int ip_dest2 = b8tob16(ip_fields[18], ip_fields[19]);
    sumarr[sumcnt++] = ip_dest2;

    // 将ip头部中的协议转变格式,并且前八位补0
    int ip_prol = (int)ip_fields[9];
    sumarr[sumcnt++] = ip_prol;

    // 将udp两个端口转变格式
    int src_port = b8tob16(udp_fields[0], udp_fields[1]);
    sumarr[sumcnt++] = src_port;
    
    int dest_port = b8tob16(udp_fields[2], udp_fields[3]);
    sumarr[sumcnt++] = dest_port;

    // 将udp长度转变格式
    int udp_l = b8tob16(udp_fields[4], udp_fields[5]);
    sumarr[sumcnt++] = udp_l;
    sumarr[sumcnt++] = udp_l;

    // 将校验和置为0
    sumarr[sumcnt++] = 0;

    // 将udp中的数据转变格式
    udp_data = (char *)((char *)uh + 8);
    int cnt = udp_l-8;
    printk("cnt = %d\n", cnt);
    if(cnt%2 == 0)
    {
        int cnt2 = cnt/2;
        int m = 0;
        int n = 0;
        while (cnt2 > 0)
        {
            ud[n] = b8tob16(udp_data[m], udp_data[m+1]);
            printk("udp_data = %2x%2x\n", udp_data[m],udp_data[m+1]);
            sumarr[sumcnt++] = ud[n];
            printk("ud = %d\n", ud[n]);
            cnt2--;
            n++;
            m+=2;
        } 
    }
    else
    {
        int cnt2 = cnt/2;
        int m = 0;
        int n = 0;
        while (cnt2 > 0)
        {;
            ud[n] = b8tob16(udp_data[m], udp_data[m+1]);
            sumarr[sumcnt++] = ud[n];
            cnt2--;
            n++;
            m+=2;
        } 
        ud[n] = b8tob16(udp_data[m], '0');
        sumarr[sumcnt++] = ud[n];
    }

    // 反码求和
    int i ;
    for (i = 0; i < sumcnt; i++)
    {
        printk("One of the members of the check: %02x\n", sumarr[i]);
        //printk("One of the members of the check: %d\n", sumarr[i]);
        sum += sumarr[i];
        if (sum > MAX_NUM)
        {
            sum = sum % MAX_NUM +1;
        }

    }

    sum = 65535 - sum;
    printk("sumcnt = %d\n", sumcnt);
    printk("My check = %d\n", sum);
    printk("My check(hex) = %02x\n", sum);
    vfree(ud);
    vfree(sumarr);
    return sum;
}


unsigned short  checksum(unsigned short* buffer, int size)
{
    unsigned long cksum = 0;
    while(size>1)
    {
        cksum += *buffer++;
        size -= sizeof(unsigned short);
    }
    if(size)
    {
        cksum += *(unsigned char*)buffer;
    }
    cksum = (cksum>>16) + (cksum&0xffff); 
    cksum += (cksum>>16); 
    return (unsigned short)(~cksum);
}

// Define our own hook function
// The parameters of this function has been updated.
// This is the prototype of this function
// unsigned int hook_func(unsigned int hooknum, 
//                          struct sk_buff *skb, 
//                          const struct net_device *in,
//                          const struct net_device *out, 
//                          int (*okfn)(struct sk_buff *))


unsigned int my_hook_func(void *priv,struct sk_buff *skb,const struct nf_hook_state *state)
{
	// 这里是ip头
    ip_header = (struct iphdr *)skb_network_header(skb);
	

    if ((ip_header->daddr == in_aton("192.168.2.192"))&&ip_header->protocol == 17)   
	    {
		if((ip_header->saddr == in_aton("192.168.1.64")) ||(ip_header->saddr == in_aton("192.168.1.68")))			
		
			return NF_QUEUE;
	    }
     return NF_ACCEPT;
}




int init_module()
{
    // Init the struct nfho
    nfho.hook = my_hook_func;
    //nfho.hooknum = NF_INET_LOCAL_OUT;
    //nfho.hooknum = NF_INET_LOCAL_IN;
    //nfho.hooknum = NF_INET_POST_ROUTING;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    //nfho.hooknum = NF_INET_FORWARD;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;

    // Register the hook function by the struct nfho
    // This function has already update t2o nf_register_net_hook from nf_register_hook
    nf_register_net_hook(&init_net, &nfho);
	//nf_register_hook(&nfho);
    return 0;
}
void cleanup_module()
{
    // Unregister the hook function
    // This function has also already update to nf_unregister_net_hook from nf_unregister_hook
    nf_unregister_net_hook(&init_net, &nfho);
    //nf_unregister_hook(&nfho);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("UDP");


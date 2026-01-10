// +build ignore

#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <bpf/bpf_endian.h>

SEC("xdp")
int drop_packet(struct xdp_md *ctx) {
    int drop = 0;

    void *data = (void*) (long) ctx->data;
    void *data_end = (void*) (long) ctx->data_end;

    struct ethhdr *eth = data;

    //mandatory check
     if (data + sizeof(struct ethhdr) > data_end)
        return XDP_PASS;

     // check if it is an IP packet
     if (bpf_ntohs(eth->h_proto) == ETH_P_IP)   {

        struct iphdr *iph = data + sizeof(struct ethhdr);

        if ((void*) iph + sizeof(struct iphdr) > data_end)
            return XDP_PASS;

        // check if it TCP Protocol
        if (iph->protocol != 6)
            return XDP_PASS;

        struct tcphdr *tcp = (void*)iph + sizeof(struct iphdr);

        if ((void*)tcp + sizeof(struct tcphdr) > data_end)
            return XDP_PASS;

        __u16 port = bpf_ntohs(tcp->dest);
        if (port == 4040){
            bpf_printk("Dropped a packet on 4040\n");
             return XDP_DROP;
        }
     }
    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
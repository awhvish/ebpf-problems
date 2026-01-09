struct xdp_md {
    __u32 data;
    __u32 data_end;
    __u32 data_meta;
    __u32 ingress_ifindex;
    __u32 rx_queue_index;
    __u32 egress_ifindex;
};

SEC("xdp")
int drop_packet(struct xdp_md *ctx) {
    bool drop;
    // Write drop logic -> if TCP Packet
    if (drop) return XDP_DROP;
    return XDP_PASS;
}
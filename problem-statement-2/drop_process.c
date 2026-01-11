#include"vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#define TARGET_PROCESS "myprocess"
#define TARGET_LEN 9

static __always_inline int is_my_process(char s1[]) {
    char s2[] = TARGET_PROCESS;
    int n = TARGET_LEN;

    #pragma unroll
    for(int i=0;i<n;i++) {
        if (s1[i]!=s2[i]) return 0;
    }
    return 1;
}

// for outgoing requests -> drops packets with name 'myprocess' if port 4040
SEC("cgroup_skb/egress")
int drop_packet(struct __sk_buff *skb){
    char comm[TASK_COMM_LEN];
    if (bpf_get_current_comm(comm, TASK_COMM_LEN)<0) {
        bpf_printk("Failed to get comm\n");
        return 1;
    }

    //allow if it is some other process
    if (!is_my_process(comm)) {
        return 1;
    }

    struct bpf_sock *sk = skb->sk;

    if (sk) {
        __u32 port = sk->dst_port;
        // it is on target port -> reject
        if (port == bpf_htons(4040)) return 0;
    }
    return 1;
}

char _license[] SEC("license") = "GPL";
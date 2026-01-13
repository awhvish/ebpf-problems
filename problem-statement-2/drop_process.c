// +build ignore

#include <linux/types.h>
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#define TARGET_PROCESS "myprocess"
#define TARGET_LEN 9 // length of process name

struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, 1);
    __type(key, __u32);
    __type(value, __u32);
} port_map SEC(".maps");


static __always_inline int is_my_process(char s1[]) {
    char s2[] = TARGET_PROCESS;
    int n = TARGET_LEN;

    #pragma unroll
    for(int i=0;i<n;i++) {
        if (s1[i]!=s2[i]) return 0;
    }
    return 1;
}

// for incoming requests -> drops packets with name 'myprocess' if port is TARGET_PORT
SEC("cgroup/bind4")
int restrict_bind_port(struct bpf_sock_addr *ctx){
    char comm[16];
    if (bpf_get_current_comm(comm, 16)<0) {
        bpf_printk("Failed to get comm\n");
        return 1; // allow on error
    }

    // allow if it's some other process
    if (!is_my_process(comm)) {
        return 1;
    }
    __u32 key;
     __u32 *userspace_port = bpf_map_lookup_elem(&port_map, &key);

     // defaults to 4040
      __u32 target_port = (userspace_port && *userspace_port != 0) ? *userspace_port : 4040;

    // for our target process, only allow binding to ALLOWED_PORT
    if (ctx->user_port == bpf_htons(target_port)) {
        bpf_printk("BPF: Allowing %s to bind to port %d\n", comm, target_port);
        return 1;
    }

    bpf_printk("BPF: Blocking %s from binding to port %d\n", comm, bpf_ntohs(ctx->user_port));
    return 0;
}

char _license[] SEC("license") = "GPL";
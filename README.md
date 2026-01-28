# eBPF Network Security Filter

A high-performance network packet filter leveraging eBPF (Extended Berkeley Packet Filter) for customizable traffic control at the kernel level. Built with C for eBPF programs and Go for userspace management using `libbpf` and Cilium eBPF libraries.

## Overview

This project implements a flexible network security framework that enables real-time packet filtering and process-level network access control without modifying kernel code or adding latency to the network stack.

### Key Features

- **Protocol-Based Filtering**: Selective TCP/UDP packet filtering at the XDP (eXpress Data Path) layer for maximum performance
- **Port-Based Blocking**: Dynamic port blocking capabilities with runtime configuration
- **Process Whitelisting**: Process-level network access control using cgroup-based eBPF programs
- **Kernel-Userspace Communication**: Bidirectional communication channel using BPF maps for dynamic rule updates
- **Zero-Copy Packet Processing**: Leverages XDP for packet decisions at the earliest possible point in the network stack

## Architecture

### Components

1. **XDP Packet Filter** ([`problem-statement-1/drop_tcp.c`](problem-statement-1/drop_tcp.c))
   - Attaches to network interface at XDP hook point
   - Performs TCP protocol identification and port-based filtering
   - Utilizes BPF maps for dynamic port configuration from userspace

2. **Cgroup Process Filter** ([`problem-statement-2/drop_process.c`](problem-statement-2/drop_process.c))
   - Intercepts socket bind operations at the cgroup level
   - Implements process name-based whitelisting/blacklisting
   - Allows fine-grained control over which processes can bind to specific ports

3. **Userspace Controller** (Go)
   - Loads and manages eBPF programs using Cilium eBPF library
   - Provides interactive configuration interface
   - Handles graceful attachment/detachment of eBPF programs

### Communication Flow

```
User Input (Go) → BPF Map Update → Kernel Space (eBPF) → Packet Decision (XDP_PASS/XDP_DROP)
                                                     ↓
                                            Debug Logs (trace_pipe)
```

## Technical Implementation

### TCP Port-Based Filtering

The XDP program performs the following packet processing pipeline:

1. **Ethernet Header Validation**: Verifies packet boundaries and extracts protocol type
2. **IP Header Processing**: Validates IPv4 packets and checks protocol field
3. **TCP Header Extraction**: Parses TCP header to extract destination port
4. **Map Lookup**: Queries BPF map for blocked port configuration
5. **Verdict**: Returns `XDP_DROP` for matched traffic, `XDP_PASS` otherwise

```c
// Kernel-space BPF map for userspace communication
struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, 1);
    __type(key, __u32);
    __type(value, __u32);
} port_map SEC(".maps");
```

### Process-Level Access Control

Uses cgroup eBPF hooks to intercept socket bind calls and enforce process-based policies:

- Extracts process name using `bpf_get_current_comm()`
- Compares against whitelist using compile-time verified loops
- Allows/denies bind operations based on configured rules

## Quick Start

### Prerequisites

- Linux kernel 5.10+ with eBPF support enabled
- Go 1.18+
- Root/sudo privileges for eBPF operations

### Build and Run

**1. TCP Port Filter**
```bash
cd problem-statement-1
go generate .
go build -o tcp-filter .
sudo ./tcp-filter
# Enter port number when prompted (e.g., 4040)
```

**2. Process-Based Filter**
```bash
cd problem-statement-2
go generate .
go build -o process-filter .
sudo ./process-filter
# Enter allowed port for 'myprocess'
```

### Monitoring

Monitor eBPF kernel logs in real-time:
```bash
sudo cat /sys/kernel/debug/tracing/trace_pipe
```

## Demonstrations

- **Video Walkthroughs**: [Google Drive](https://drive.google.com/drive/folders/1NVEwvcEzd-zaS6Q5YoPwql5hIMZWXf7A?usp=sharing)
- **Problem Statements**: 
  - [TCP Port Blocking](https://github.com/awhvish/ebpf-problems/blob/master/problem-statement-1/read.md)
  - [Process Whitelisting](https://github.com/awhvish/ebpf-problems/blob/master/problem-statement-2/read.md)

## Technical Details

### Stack Technologies

- **C**: eBPF kernel programs with BPF helper functions
- **Go**: Userspace management and control plane
- **libbpf/Cilium eBPF**: BPF program loading and map management
- **XDP**: eXpress Data Path for high-performance packet processing
- **cgroup v2**: Process-level network policy enforcement

### Performance Characteristics

- **Zero-copy packet processing**: Decisions made before sk_buff allocation
- **Minimal CPU overhead**: eBPF verifier ensures safe, bounded execution
- **No kernel recompilation**: Dynamic loading of eBPF programs
- **Scalable filtering**: O(1) map lookups for port matching

## Learning Resources

This project was developed with insights from:
- **Learning eBPF** by Liz Rice
- Linux kernel documentation
- Cilium eBPF library examples


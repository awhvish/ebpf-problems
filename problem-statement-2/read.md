### Problem Statement 2

Write an eBPF code to allow traffic only at a specific TCP port (default 4040) for a given process name (for e.g, "myprocess"). 
All the traffic to all other ports for only that process should be dropped.

Let,
ALLOWED_PORT:  TCP port -> configurable from userspace
TARGET_PROCESS: `myprocess` - The process that is allowed

To Test:

1. packet with TARGET_PROCESS binds to ALLOWED_PORT: accept
    - 
2. packet with TARGET_PROCESS arrives at some other port: reject
    - 
3. packet with any other process to ALLOWED_PORT or any other port: accept
    - 
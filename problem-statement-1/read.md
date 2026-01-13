### Problem Statement 1
Write an eBPF code to drop the TCP packets on a port (def: 4040). Additionally, if you can make the port number configurable from the userspace, that will be a big plus.

To Test:
1. send TCP packets on selected port without ebpf (check server responds)
    - start a nc server `nc -l 4045`
    - check if packets are being dropped: nothing on debug terminal -> packets not dropped
   
2. load ebpf into the kernel (run the main.go file) and check if TCP packets are being dropped
    - start the main.go file and set port as 4045 (configurable from userspace)
    - packets are being dropped
   
3. since only TCP packets are to be blocked, test on a UDP nc server as well (packets should be allowed)
    - start a nc server that accepts udp `nc -l -u -p 4045`
    - check if packets are being dropped: no packets dropped. 

package main

import (
	"log"
	"net"
	"os"
	"os/signal"
	"syscall"

	"github.com/cilium/ebpf/link"
	"github.com/cilium/ebpf/rlimit"
)

func main() {
	// Remove resource limits for kernels <5.11.
	if err := rlimit.RemoveMemlock(); err != nil {
		log.Fatal("Removing memlock:", err)
	}

	objs := mainObjects{}
	if err := loadMainObjects(&objs, nil); err != nil {
		log.Fatal("Loading main objects:", err)
	}
	defer objs.Close()

	iface, err := net.InterfaceByName("lo")
	if err != nil {
		log.Fatal("Failed to find lo interface : ", err)
	}

	l, err := link.AttachXDP(link.XDPOptions{
		Program:   objs.DropPacket,
		Interface: iface.Index,
	})
	if err != nil {
		log.Fatal("Failed Attaching XDP: ", err)
	}
	defer l.Close()

	log.Println("Blocking TCP packets on Port 4040. CTRL+C to stop.")

	stop := make(chan os.Signal, 1)
	signal.Notify(stop, os.Interrupt, syscall.SIGTERM)
	<-stop
}

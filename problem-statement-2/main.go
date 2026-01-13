package main

import (
	"log"
	"os"
	"os/signal"
	"syscall"

	"github.com/cilium/ebpf"
	"github.com/cilium/ebpf/link"
	"github.com/cilium/ebpf/rlimit"
)

func main() {
	if err := rlimit.RemoveMemlock(); err != nil {
		log.Fatal("Removing memlock:", err)
	}
	objs := mainObjects{}

	if err := loadMainObjects(&objs, nil); err != nil {
		log.Fatalf("Error loading object: %s", err)
	}
	defer objs.Close()

	cgroupPath := "/sys/fs/cgroup"
	l, err := link.AttachCgroup(link.CgroupOptions{
		Path:    cgroupPath,
		Program: objs.RestrictBindPort,
		Attach:  ebpf.AttachCGroupInet4Bind,
	})
	if err != nil {
		log.Fatalf("Error attaching cgroup: %s", err)
	}
	defer l.Close()
	log.Printf("Successfully attached to %s. Blocking 'myprocess' on port 4040...", cgroupPath)

	notify := make(chan os.Signal, 1)
	signal.Notify(notify, os.Interrupt, syscall.SIGTERM)
	<-notify
}

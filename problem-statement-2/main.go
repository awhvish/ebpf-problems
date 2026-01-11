package main2

import (
	"log"

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

}

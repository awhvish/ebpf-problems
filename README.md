## Run 
To Run ebpf:

```code
  go generate .
  go build -o myfile .
  sudo ./myfile
```

For debug logs:
```code
  sudo cat /sys/kernel/debug/tracing/trace_pipe
```

## Problem Statement 1

Test Files: [Here](https://github.com/awhvish/ebpf-problems/blob/master/problem-statement-1/read.md)

## Problem Statement 2
Test Files: [Here](https://github.com/awhvish/ebpf-problems/blob/master/problem-statement-2/read.md)

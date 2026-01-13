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

## Problem Statement 2

```go
    package main
    
    import "fmt"
    
    func main() {
        cnp := make(chan func(), 10)
        for i := 0; i < 4; i++ {
            go func() {
                for f := range cnp {
                    f()
                }
            }()
        }
        cnp <- func() {
            fmt.Println("HERE1")
        }
        fmt.Println("Hello")
    }
```

### Why `HERE1` is not being printed?
`cnp <- func(){...}` passes a function that is supposed to print 'HERE1', but since it is a goroutine our program never waits for the goroutine to finish working.
that is, it passed function to channel,  prints "Hello" and main function ends.
To see the `HERE1` we can add a wait group or a sleep of 1 second.

### Significance of the for loop with 4 iterations?
Launches 4 independent goroutines with the help of anonymous functions.

### Significance of make(chan func(), 10)?
It creates a buffered channel that can hold 10 functions in the memory simultaneously.
The sender can keep sending upto 10 functions, until it has to wait for one of them to finish and accept new function.
If the channel were to be of no buffer (`make (chan func())`), then it is forced to wait for each and every task at hand, and hence we would see 'HERE1' printed.

### Use cases of Worker Pool pattern?
Any program that requires asynchronous management can use this pattern.
Specifically, In checkout pages, after successful orders we are made to wait for certain seconds in many applications to finish all API calls and then redirects to success.
We can use this pattern to remove the entire waiting period of those seconds, and redirect when all the goroutines finish their respective functions.

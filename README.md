#OS(assignment 2) : build a mini kernel
##  __TEAMMATES__
1. Trần Đình Tiến
2. Nhan Ngọc Thiện
3. Nguyễn Việt Khoa
4. Trần Minh Hiếu

## Assign

### Hiếu + Thiện: queue.c, os.c, sched.c, loader.c
### Tiến + Khoa: timer.c, cpu.c, paging.c, mem.c

## Summary

### Header files
1. timer.h: Define the timer for the whole system.
2. cpu.h: Define functions used to implement the virtual CPU
3. queue.h: Functions used to implement queue which holds the PCB of processes struct (queue t) for a priority queue 
4. sched.h: Define functions used by the scheduler
5. mem.h: Functions used by Virtual Memory Engine.
   - struct mem_stat
6. loader.h: Functions used by the loader which load the program from disk to memory
7. common.h: Define structs and functions used everywhere in the OS.

### C files
1. timer.c: Implement the timer.
   - The CPU runs processes in round-robin style. Each process is allowed to run upto a given period of time.
2. cpu.c: Implement the virtual CPU.
   - CALC: calculate with out argument
   - ALLOC
   - FREE
   - READ
   - WRITE
3. queue.c: Implement operations on (priority) queues.
   - enqueue() & dequeue():put a new PCB to the queue
   - and get a PCB with the highest priority out of the queue
   - add_queue(): add process form loader to ready_queue
4. paging.c: Use to check the functionality of Virtual Memory Engine.
5. os.c: The whole OS starts running from this file.
6. loader.c: Implement the loader
   - loader()
7. sched.c: Implement the scheduler
   - get proc(): gets PCB of a process waiting at ready queue 
   - put_proc(): put process to run_queue
8. mem.c: Implement RAM and Virtual Memory Engine
   - get page table() : Find the page table given a segment index of a process.
   - translate(): uses get page table() function to translate a virtual address to physical address.
   - free_mem():
   - alloc_mem():

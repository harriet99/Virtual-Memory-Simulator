# Virtual-Memory-Simulator

### Description for each file
- va_splitting.h - Break down a virtual address into its components.
- mmu.c - Initialize any system- and memory access-related bookkeeping.
- proc.c - Initialize any process-related bookkeeping.
- page_fault.c - Implement the page fault handler.
- page_replacement.c - Implement the frame eviction algorithms FIFO and Clock Sweep algorithm. • stats.c - Calculate the Average Memory Access Time (AMAT)
- mmu.h - Defines the structures used by mmu.c
- proc.h - Defines the structures used by proc.c
- pagesim.h - Defines simulation parameters as well as global structures.
- pagesim.c - Reads a trace file of memory operations and calls each operation’s corresponding function implemented in proc.c.
- stats.h - Defines parameters that can be used when calculating AMAT.
- swap.c - Initializes functions to support a queue that are used in swapops.c
- swapops.c - Initializes functions to keep track of the frames swapped out to physical memory. Discussed in section 8
- types.h - Defines different types that are used throughout the simulation
- util.c - Initializes a random function used for the random replacement algorithm

### Organization of physical memory showing frames and frame entries
![Screen Shot 2022-08-27 at 1 24 03 AM](https://user-images.githubusercontent.com/79822409/187016168-569025e4-f325-452a-a197-e9f9d5083589.png)

### Simulator Process Diagram
![Screen Shot 2022-08-27 at 1 26 24 AM](https://user-images.githubusercontent.com/79822409/187016221-6804c076-1cc0-476a-89c2-b1c6b04f305b.png)

### How to Run the Code
- Environment: the code will need to compile under Ubuntu 20.04 LTS.
- Compiling and Running: Makefile is there and will run gcc for you.
- Example codes to run:
  - $ make
  - $./vm-sim -i traces/<trace >.trace -rrandom
  <br />

  - $ make
  - $./vm-sim -i traces/<trace >.trace -rclocksweep
  <br />
  
  - $ make
  - $./vm-sim -i traces/<trace >.trace -rfifo

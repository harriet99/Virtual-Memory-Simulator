#include "proc.h"
#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 3 --------------------------------------
 * Checkout PDF section 4 for this problem
 * 
 * This function gets called every time a new process is created.
 * You will need to allocate a frame for the process's page table using the
 * free_frame function. Then, you will need update both the frame table and
 * the process's PCB. 
 * 
 * @param proc pointer to process that is being initialized 
 * 
 * HINTS:
 *      - pcb_t: struct defined in pagesim.h that is a process's PCB.
 *      - You are not guaranteed that the memory returned by the free frame allocator
 *      is empty - an existing frame could have been evicted for our new page table.
 * ----------------------------------------------------------------------------------
 */
void proc_init(pcb_t *proc) {
    // TODO: initialize proc's page table.
    pfn_t freed = free_frame(); //get free frame for the process's page table
    memset(mem + freed * PAGE_SIZE, 0, PAGE_SIZE); //zero-out the memory
    frame_table[freed].protected = 1;
    proc->saved_ptbr = freed; //set process to PTBR
}   

/**
 * --------------------------------- PROBLEM 4 --------------------------------------
 * Checkout PDF section 5 for this problem
 * 
 * Switches the currently running process to the process referenced by the proc 
 * argument.
 * 
 * Every process has its own page table, as you allocated in proc_init. You will
 * need to tell the processor to use the new process's page table.
 * 
 * @param proc pointer to process to become the currently running process.
 * 
 * HINTS:
 *      - Look at the global variables defined in pagesim.h. You may be interested in
 *      the definition of pcb_t as well.
 * ----------------------------------------------------------------------------------
 */
void context_switch(pcb_t *proc) {
    // TODO: update any global vars and proc's PCB to match the context_switch.
    PTBR = proc->saved_ptbr; //set PTBR to the ptbr of the process
}

/**
 * --------------------------------- PROBLEM 8 --------------------------------------
 * Checkout PDF section 8 for this problem
 * 
 * When a process exits, you need to free any pages previously occupied by the
 * process.
 * 
 * HINTS:
 *      - If the process has swapped any pages to disk, you must call
 *      swap_free() using the page table entry pointer as a parameter.
 *      - If you free any protected pages, you must also clear their"protected" bits.
 * ----------------------------------------------------------------------------------
 */
void proc_cleanup(pcb_t *proc) {
    // TODO: Iterate the proc's page table and clean up each valid page
    pte_t *proc_base = (pte_t *)(mem + proc->saved_ptbr * PAGE_SIZE); //the base of the proc's page table
    for (size_t i = 0; i < NUM_PAGES; i++) {
        if ((proc_base + i)->valid) {
            frame_table[(proc_base + i)->pfn].mapped = 0;
            (proc_base + i)->valid = 0;
            (proc_base + i)->dirty = 0;
        }
        if (swap_exists(proc_base + i)) {
            swap_free(proc_base + i);
        }
    }
    frame_table[proc->saved_ptbr].protected = 0; //free the page table in the frame table
    frame_table[proc->saved_ptbr].mapped = 0;
}

#pragma GCC diagnostic pop

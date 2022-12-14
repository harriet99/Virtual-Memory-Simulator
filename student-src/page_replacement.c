#include "types.h"
#include "pagesim.h"
#include "mmu.h"
#include "swapops.h"
#include "stats.h"
#include "util.h"

pfn_t select_victim_frame(void);

pfn_t last_evicted = 0;

/**
 * --------------------------------- PROBLEM 7 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Make a free frame for the system to use. You call the select_victim_frame() method
 * to identify an "available" frame in the system (already given). You will need to 
 * check to see if this frame is already mapped in, and if it is, you need to evict it.
 * 
 * @return victim_pfn: a phycial frame number to a free frame be used by other functions.
 * 
 * HINTS:
 *      - When evicting pages, remember what you checked for to trigger page faults 
 *      in mem_access
 *      - If the page table entry has been written to before, you will need to use
 *      swap_write() to save the contents to the swap queue.
 * ----------------------------------------------------------------------------------
 */
pfn_t free_frame(void) {
    pfn_t victim_pfn;
    victim_pfn = select_victim_frame();

    // TODO: evict any mapped pages.
    if (frame_table[victim_pfn].mapped) {
        pcb_t *pointer_to_process = frame_table[victim_pfn].process;
        pte_t *pointer_victim_pte = (pte_t*)(mem + (pointer_to_process->saved_ptbr * PAGE_SIZE)) + frame_table[victim_pfn].vpn;
    
        if (pointer_victim_pte->dirty) {
            stats.writebacks++;
            swap_write(pointer_victim_pte, mem + victim_pfn * PAGE_SIZE);
            pointer_victim_pte->dirty = 0;
        }

        pointer_victim_pte->valid = 0;
    }

    frame_table[victim_pfn].mapped = 0; //unmapped
    return victim_pfn;
}



/**
 * --------------------------------- PROBLEM 9 --------------------------------------
 * Checkout PDF section 7, 9, and 11 for this problem
 * 
 * Finds a free physical frame. If none are available, uses either a
 * randomized, FCFS, or clocksweep algorithm to find a used frame for
 * eviction.
 * 
 * @return The physical frame number of a victim frame.
 * 
 * HINTS: 
 *      - Use the global variables MEM_SIZE and PAGE_SIZE to calculate
 *      the number of entries in the frame table.
 *      - Use the global last_evicted to keep track of the pointer into the frame table
 * ----------------------------------------------------------------------------------
 */
pfn_t select_victim_frame() {
    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++) {
        if (!frame_table[i].protected && !frame_table[i].mapped) {
            return i;
        }
    }

    // RANDOM implemented for you.
    if (replacement == RANDOM) {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t last_unprotected = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                last_unprotected = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (last_unprotected < NUM_FRAMES) {
            return last_unprotected;
        }


    } else if (replacement == FIFO) {
        // TODO: Implement a FIFO algorithm here
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[last_evicted].protected) {
                pfn_t temp = last_evicted;
                last_evicted = (size_t)(last_evicted + 1) % num_entries;
                return temp;
            }
            last_evicted = (size_t)(last_evicted + 1) % num_entries;
        }
    } else if (replacement == CLOCKSWEEP) {
        // TODO: Implement a clocksweep page replacement algorithm here 
        pfn_t out = 0;
        int count = 0;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[last_evicted].protected) {
                if (count == 0) { //edge case: if all the entries' references are 1
                    count++;
                    out = last_evicted;
                }
                if (!frame_table[last_evicted].referenced) {
                    pfn_t temp = last_evicted;
                    last_evicted = (size_t)(last_evicted + 1) % num_entries;
                    return temp;
                }
            }
            frame_table[last_evicted].referenced = 0;
            last_evicted = (size_t)(last_evicted + 1) % num_entries;
        }
        last_evicted = (size_t)(out + 1) % num_entries;
        return out;
    }

    /* If every frame is protected, give up. This should never happen
       on the traces we provide you. */
    panic("System ran out of memory\n");
    exit(1);
}

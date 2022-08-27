#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* The frame table pointer. You will set this up in system_init. */
fte_t *frame_table;

/**
 * --------------------------------- PROBLEM 2 --------------------------------------
 * Checkout PDF sections 4 for this problem
 * 
 * In this problem, you will initialize the frame_table pointer. The frame table will
 * be located at physical address 0 in our simulated memory. You should zero out the 
 * entries in the frame table, in case for any reason physical memory is not clean.
 * 
 * HINTS:
 *      - mem: Simulated physical memory already allocated for you.
 *      - PAGE_SIZE: The size of one page
 * ----------------------------------------------------------------------------------
 */
void system_init(void) {
    // TODO: initialize the frame_table pointer.
    frame_table = (fte_t *)mem;
    memset(frame_table, 0, PAGE_SIZE); //zero-out
    frame_table->protected = 1; //mark first entry of the frame table as "protected"
}

/**
 * --------------------------------- PROBLEM 5 --------------------------------------
 * Checkout PDF section 6 for this problem
 * 
 * Takes an input virtual address and performs a memory operation.
 * 
 * @param addr virtual address to be translated
 * @param rw   'r' if the access is a read, 'w' if a write
 * @param data If the access is a write, one byte of data to write to our memory.
 *             Otherwise NULL for read accesses.
 * 
 * HINTS:
 *      - Remember that not all the entry in the process's page table are mapped in. 
 *      Check what in the pte_t struct signals that the entry is mapped in memory.
 * ----------------------------------------------------------------------------------
 */
uint8_t mem_access(vaddr_t addr, char rw, uint8_t data) {
    // TODO: translate virtual address to physical, then perfrom the specified operation
    stats.accesses++;
    vpn_t vpn_address = vaddr_vpn(addr); //vpn of the address
    uint16_t offset_address = vaddr_offset(addr); //offset of the address
    pte_t *pointer_to_pte = (pte_t *)(mem + PTBR * PAGE_SIZE) + vpn_address; //keep pointer to the entry

    if (!pointer_to_pte->valid) { //if the entry is invalid
        stats.page_faults++;
        page_fault(addr); //allocate the page for that address
    }

    frame_table[pointer_to_pte->pfn].mapped = 1;
    frame_table[pointer_to_pte->pfn].referenced = 1;
    frame_table[pointer_to_pte->pfn].vpn = vpn_address;
    frame_table[pointer_to_pte->pfn].process = current_process;

    /* calculate physical address with pointer_entry->pfn and offset_address */
    paddr_t physical_addr = (paddr_t)(offset_address + ((pointer_to_pte->pfn) << OFFSET_LEN));

    /* Either read or write the data to the physical address
       depending on 'rw' */
    uint8_t out = *(mem + physical_addr);
    if (rw == 'w') {
        *(mem + physical_addr) = data;
        pointer_to_pte->dirty = 1;
    }
    return out;
}

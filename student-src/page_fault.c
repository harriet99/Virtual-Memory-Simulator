#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t addr) {
   // TODO: Get a new frame, then correctly update the page table and frame table
   vpn_t vpn_address = vaddr_vpn(addr); //vpn of the address
   pte_t *pointer_to_pte = (pte_t *)(mem + PTBR * PAGE_SIZE) + vpn_address; //keep pointer to the entry

   pfn_t freed = free_frame(); //get pfn of the new frame

   pte_t *pointer_new_frame = (pte_t *)(mem + freed * PAGE_SIZE); //new frame in memory.

   if(swap_exists(pointer_to_pte)){
      swap_read(pointer_to_pte, pointer_new_frame);
   } else {
      memset(mem + freed * PAGE_SIZE, 0, PAGE_SIZE); //clear the new frame
   }

   /* update the mapping from vpn to the new pfn in the current process' page table */
   pointer_to_pte->valid = 1;
   pointer_to_pte->dirty = 0;
   pointer_to_pte->pfn = freed;
   
   /* update the flags in the corresponding frame table entry */
   frame_table[freed].mapped = 1;
   frame_table[freed].process = current_process;
   frame_table[freed].vpn = vpn_address;
}

#pragma GCC diagnostic pop

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
   // TODO: Iterate the proc's page table and clean up each valid page
   vpn_t vpn = vaddr_vpn(addr);
   pte_t *pte = ((pte_t*) (mem + (PTBR * PAGE_SIZE))) + vpn;

   pfn_t pfn = free_frame();

   pte->pfn = pfn;
   pte->valid = 1;
   pte->dirty = 0;

   frame_table[pfn].process = current_process;
   frame_table[pfn].vpn = vpn;
   frame_table[pfn].mapped = 1;
   frame_table[pfn].ref_count = 0; // for LRU

   paddr_t *new_frame = (paddr_t *) (mem + (PAGE_SIZE * pfn));

   if(swap_exists(pte)){
     swap_read(pte, new_frame);
   } else {
     memset(mem + (pfn * PAGE_SIZE), 0, PAGE_SIZE);
   }
   stats.page_faults++;
}


#pragma GCC diagnostic pop


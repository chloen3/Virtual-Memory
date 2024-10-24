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
    // TODO: evict any mapped pages.
    pfn_t selected_frame;
    selected_frame = select_victim_frame();

    if (frame_table[selected_frame].mapped) {
        pcb_t *process_of_victim = frame_table[selected_frame].process;
        pfn_t page_table_base = process_of_victim->saved_ptbr;
        pte_t *page_table_entry = ((pte_t*) (mem + (page_table_base * PAGE_SIZE))) + frame_table[selected_frame].vpn;
        
        if (page_table_entry->dirty) {
            swap_write(page_table_entry, (uint8_t *) (mem + (selected_frame * PAGE_SIZE)));
            stats.writebacks += 1;
        }
        
        page_table_entry->valid = 0;
        page_table_entry->dirty = 0;
        frame_table[selected_frame].mapped = 0;
    }

    return selected_frame;

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
        pfn_t unprotected_found = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                unprotected_found = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (unprotected_found < NUM_FRAMES) {
            return unprotected_found;
        }


    } else if (replacement == APPROX_LRU) {
    // TODO: Implement the Approximate LRU (Least Recently Used) algorithm here
    // initialize min_value with a high number, assuming it's larger than any ref_count
        uint8_t min_value = UINT8_MAX;
        uint8_t index = 0; // index of last recently used page

        for(size_t idx = 0; idx < num_entries; idx++) { // loop through all entries in the frame table to find the one with the smallest ref_count
            if(!frame_table[idx].protected && frame_table[idx].ref_count < min_value) {
                min_value = frame_table[idx].ref_count;
                index = idx;
            }
        }

        // frame_table[index].ref_count = 0; // reset the ref_count of entry
        return index;

    } else if (replacement == CLOCKSWEEP) {
        // TODO: Implement the clocksweep page replacement algorithm here 

        for (size_t i = 1; i <= 2 * num_entries + 1; i++) { // iterates through frame table
            pfn_t current_frame = (last_evicted + i) % num_entries;
            fte_t* frame = &frame_table[current_frame];
            
            if (!frame->protected) {
                pte_t *page_table = (pte_t *)((size_t)mem + ((size_t)frame->process->saved_ptbr * PAGE_SIZE));
                pte_t *current_page = &page_table[frame_table[current_frame].vpn]; // PTE for current frame

                if (current_page->referenced) {
                    current_page->referenced = 0;
                } else {
                    last_evicted = (last_evicted + i) % num_entries;
                    return last_evicted;
                }
            }

        }
    }

    /* If every frame is protected, give up. This should never happen
       on the traces we provide you. */
    panic("System ran out of memory\n");
    exit(1);
}

/**
 * --------------------------------- PROBLEM 10.2 --------------------------------------
 * Checkout PDF for this problem
 *
 * Updates the associated variables for the Approximate LRU,
 * called every time the simulator daemon wakes up.
 *
 * ----------------------------------------------------------------------------------
 */
void daemon_update(void)
{
    size_t num_entries = MEM_SIZE / PAGE_SIZE; // total number of entries in the frame table

    for (size_t i = 0; i < num_entries; i++) {
        if (frame_table[i].mapped) {
            // Calculate the PTE for the frame's process and VPN
            pte_t* pte = (pte_t*)((size_t)(frame_table[i].process->saved_ptbr * PAGE_SIZE) + (size_t)mem);
            
            // Update ref_count by shifting right and adding the current referenced bit (from the PTE)
            frame_table[i].ref_count = frame_table[i].ref_count >> 1;
            frame_table[i].ref_count = frame_table[i].ref_count | (pte[frame_table[i].vpn].referenced << 7);
            
            // Clear the referenced bit after processing
            pte[frame_table[i].vpn].referenced = 0;
        }
    }
}




#ifndef _MMU_MAP_H
#define _MMU_MAP_H

#include "types.h"

// A page table (at any level): a pointer to its 512 descriptors.
typedef uint64 pt_entry_t;
typedef pt_entry_t* pagetable_t;

// Add more typedefs here, if you find them useful (for example, for virtual
// and physical addresses).

// Declare your page table functions here. Write a comment for each one that
// says what it does, what its arguments mean, what it returns, and in which
// cases it fails. Your API needs functions that:
//
// 1. create a page table;
// 2. map one virtual page to one physical page, allocating any missing tables
//    on the way down, and rejecting duplicate and invalid mappings;
// 3. map a contiguous range of virtual pages to contiguous physical pages;
// 4. walk a page table to find the last-level descriptor for a virtual
//    address; and
// 5. translate a virtual address into a physical address.

#endif  // _MMU_MAP_H

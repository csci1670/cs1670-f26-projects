#ifndef _MMU_TEST_SYNTH_H
#define _MMU_TEST_SYNTH_H

#include "types.h"

// PROVIDED: a synthetic page table, for testing your page table walk.
//
// mmu_test_generate_synth_pt() builds a four-level page table by hand, without
// using any of your page table code, and gives you a list of test cases: a
// virtual address each, and what a correct software walk of the synthetic page
// table finds for it. Some of the virtual addresses are mapped, and some are
// not.

// One test case for the synthetic page table.
struct synth_pt_case {
  const char* what;  // what this case tests, for your pass and fail messages
  uint64 va;         // the virtual address to walk or translate
  bool mapped;       // whether a correct walk finds a valid mapping for va
  uint64 desc;       // if mapped: the last-level descriptor for va
  uint64 pa;         // if mapped: the physical address that va translates to
};

// Builds the synthetic page table, and returns a pointer to its top-level
// table. Pass this pointer to your walk and translate functions wherever they
// expect a top-level page table: it has the same type as pagetable_t. Sets
// *cases to an array of test cases, and *ncases to the number of cases.
//
// - Each call builds the page table from scratch, so a test that changes it by
//   accident cannot affect the next test.
// - Treat the page table as read-only: don't add mappings to it.
// - Its tables are part of the kernel image, not of the kalloc() pool, so
//   never kfree() them, or destroy the page table.
// - The physical addresses in its last-level descriptors are made up. Compare
//   them, but never read or write memory at them.
uint64* mmu_test_generate_synth_pt(const struct synth_pt_case** cases,
                                   uint64* ncases);

#endif  // _MMU_TEST_SYNTH_H

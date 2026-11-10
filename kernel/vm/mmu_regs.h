#ifndef _MMU_REGS_H
#define _MMU_REGS_H

#include "memlayout.h"
#include "types.h"
#include "vm/mmu_macros.h"

// Every page table holds this many 64-bit descriptors.
#define TABLE_ENTRIES (PAGE_SIZE / sizeof(uint64))

// Descriptor bits. Appendix A of the handout describes them in detail.

// Bit 0 (valid): if 0, the descriptor is invalid, and any access through it
// faults.
#define DESC_VALID (1ULL << 0)

// Bit 1 (type): in a top-level, 2nd-level, or 3rd-level table, 1 means that
// the descriptor points to the next table down (0 would map a whole block). In
// a last-level table, this bit must be 1.
#define DESC_TABLE (1ULL << 1)
#define DESC_PAGE (1ULL << 1)

// Bits 47-12 (output address): the physical address of the next table, or of
// the page.
#define DESC_ADDR_MASK 0x0000FFFFFFFFF000ULL

// Bits 4-2 of a page descriptor (AttrIndx): which 8-bit slot of MAIR_EL1
// describes the memory type of this page.
#define DESC_ATTRINDX(i) (((uint64)(i) & 0b111) << 2)

// Bits 7-6 of a page descriptor (AP): who can read and write this page.
// NOTE: The hardware never lets EL1 execute a page that EL0 can write, so
// kernel code must not sit in a page mapped with DESC_AP_ALL_RW.
#define DESC_AP_EL1_RW (0b00ULL << 6)  // EL1 read/write, EL0 no access
#define DESC_AP_ALL_RW (0b01ULL << 6)  // EL1 and EL0 read/write
#define DESC_AP_EL1_RO (0b10ULL << 6)  // EL1 read-only, EL0 no access
#define DESC_AP_ALL_RO (0b11ULL << 6)  // EL1 and EL0 read-only

// Bits 9-8 of a page descriptor (SH): shareability of this page.
#define DESC_SH_NON (0b00ULL << 8)
#define DESC_SH_OUTER (0b10ULL << 8)
#define DESC_SH_INNER (0b11ULL << 8)

// Bit 10 of a page descriptor (AF): access flag. If this bit is 0, the first
// access to the page faults.
#define DESC_AF (1ULL << 10)

// Bit 53 of a page descriptor (PXN): 1 = the page cannot be executed in EL1.
#define DESC_NX_KERNEL (1ULL << 53)

// Bit 54 of a page descriptor (UXN): 1 = the page cannot be executed in EL0.
#define DESC_NX_USER (1ULL << 54)

// Default attributes, for example for your tests in Quest 2.
// DESC_DEFAULT_PAGE_ATTR maps a page that only EL1 can access (because
// DESC_AP_EL1_RW is 0).
#define DESC_DEFAULT_TABLE_ATTR (DESC_VALID | DESC_TABLE)
#define DESC_DEFAULT_PAGE_ATTR \
  (DESC_VALID | DESC_PAGE | DESC_ATTRINDX(0) | DESC_SH_INNER | DESC_AF)

// MAIR index 0: normal memory
//            1: device memory
#define MAIR_IDX_NORMAL 0
#define MAIR_IDX_DEVICE 1

// Attributes for a table descriptor, in a top-level, 2nd-level, or 3rd-level
// table:
//   DESC_VALID |  // valid descriptor
//   DESC_TABLE    // points to the next table down
#define DESC_TABLE_ATTR (DESC_VALID | DESC_TABLE)

// Attributes for a last-level descriptor of a code page:
//   DESC_VALID |                      // valid descriptor
//   DESC_PAGE |                       // page descriptor (same bit as
//   DESC_TABLE) DESC_ATTRINDX(MAIR_IDX_NORMAL) |  // normal RAM (MAIR slot 0)
//   DESC_AP_ALL_RO |                  // read-only in EL1 and EL0
//   DESC_SH_INNER |                   // inner shareable
//   DESC_AF |                         // access flag: no fault on first access
//   DESC_NX_KERNEL                    // EL1 cannot execute; EL0 can
#define DESC_CODE_PAGE_ATTR                                                   \
  (DESC_VALID | DESC_PAGE | DESC_ATTRINDX(MAIR_IDX_NORMAL) | DESC_AP_ALL_RO | \
   DESC_SH_INNER | DESC_AF | DESC_NX_KERNEL)

// Attributes for a last-level descriptor of a data page:
//   DESC_VALID |                      // valid descriptor
//   DESC_PAGE |                       // page descriptor (same bit as
//   DESC_TABLE) DESC_ATTRINDX(MAIR_IDX_NORMAL) |  // normal RAM (MAIR slot 0)
//   DESC_AP_ALL_RW |                  // read/write in EL1 and EL0
//   DESC_SH_INNER |                   // inner shareable
//   DESC_AF |                         // access flag set: no fault on first
//   DESC_NX_KERNEL                    // EL1 cannot execute DESC_NX_USER
//                                     // EL0 cannot execute
#define DESC_DATA_PAGE_ATTR                                                   \
  (DESC_VALID | DESC_PAGE | DESC_ATTRINDX(MAIR_IDX_NORMAL) | DESC_AP_ALL_RW | \
   DESC_SH_INNER | DESC_AF | DESC_NX_KERNEL | DESC_NX_USER)

// Attributes for a last-level descriptor of a kernel page:
//   DESC_VALID |                      // valid descriptor
//   DESC_PAGE |                       // page descriptor
//   DESC_ATTRINDX(MAIR_IDX_NORMAL) |  // normal RAM (MAIR slot 0)
//   DESC_AP_EL1_RW |                  // read/write in EL1, no access in EL0
//   DESC_SH_INNER |                   // inner shareable
//   DESC_AF |                         // access flag: no fault on first access
//   DESC_NX_USER                      // EL0 cannot execute
// PXN is clear, so EL1 can execute these pages. UXN must be set even though
// EL0 has no access: the AP bits only control reads and writes, so a page
// with UXN clear would be execute-only for EL0, and a process could run
// kernel code by jumping into it!
#define DESC_KERN_PAGE_ATTR                                                   \
  (DESC_VALID | DESC_PAGE | DESC_ATTRINDX(MAIR_IDX_NORMAL) | DESC_AP_EL1_RW | \
   DESC_SH_INNER | DESC_AF | DESC_NX_USER)

// Attributes for a last-level descriptor of a device memory page:
//   DESC_VALID |                      // valid descriptor
//   DESC_PAGE |                       // page descriptor (same bit as
//   DESC_TABLE) DESC_ATTRINDX(MAIR_IDX_DEVICE) |  // MMIO (MAIR slot 1)
//   DESC_AP_EL1_RW |                  // read/write in EL1, no access in EL0
//   DESC_SH_INNER |                   // inner shareable
//   DESC_AF |                         // access flag: no fault on first access
//   DESC_NX_KERNEL                    // EL1 cannot execute DESC_NX_USER
//                                     // EL0 cannot execute
#define DESC_DEV_PAGE_ATTR                                                    \
  (DESC_VALID | DESC_PAGE | DESC_ATTRINDX(MAIR_IDX_DEVICE) | DESC_AP_EL1_RW | \
   DESC_SH_INNER | DESC_AF | DESC_NX_KERNEL | DESC_NX_USER)

// Put the 8-bit memory attribute attr in slot idx (0-7) of MAIR_EL1.
#define MAIR_ATTR(idx, attr) (((uint64)(attr) & 0xFF) << (8 * (idx)))

// MAIR_EL1 holds the memory types (normal or device memory, and how normal
// memory is cached) that descriptors refer to with their AttrIndx bits.
//
// If MAIR_VALUE stays 0, all memory is device memory, and normal kernel code
// can take alignment faults.
#define MAIR_VALUE 0ULL

// ID_AA64MMFR0_EL1 bits 3-0 (PARange): the physical address size of this CPU.
#define MMFR_PARANGE_POS 0
#define MMFR_PARANGE_MASK (0b1111ULL << MMFR_PARANGE_POS)

// TCR_EL1: translation control for the TTBR0 (low) address range.
#define TCR_T0SZ (16ULL << 0)  // bits 5-0: 64 - 16 = 48-bit addresses
#define TCR_IRGN0_WBWA \
  (0b01ULL << 8)  // bits 9-8: inner write-back caching for table walks
#define TCR_ORGN0_WBWA \
  (0b01ULL << 10)  // bits 11-10: outer write-back caching for table walks
#define TCR_SH0_INNER \
  (0b11ULL << 12)  // bits 13-12: table walks are inner shareable
#define TCR_TG0_4K (0b00ULL << 14)  // bits 15-14: 4 KB granule
#define TCR_EPD1 (1ULL << 23)  // bit 23: 1 = do not walk TTBR1 (high range)
#define TCR_IPS_POS 32  // bits 34-32: physical address size, from PARange
#define TCR_VALUE                                                            \
  (TCR_T0SZ | TCR_IRGN0_WBWA | TCR_ORGN0_WBWA | TCR_SH0_INNER | TCR_TG0_4K | \
   TCR_EPD1)

#endif  // _MMU_REGS_H

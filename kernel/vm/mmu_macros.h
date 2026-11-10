#ifndef _MMU_MACROS_H
#define _MMU_MACROS_H

#include "memlayout.h"
#include "types.h"

// 4 KB page granule macros
#define PAGE_SHIFT 12
#define PAGE_SIZE (1ULL << PAGE_SHIFT)
#define PAGE_MASK (PAGE_SIZE - 1)

// Page alignment functions
#define PAGE_ALIGN_DOWN(x) \
  (((uint64)(x)) & ~PAGE_MASK)  // Round down to a page boundary
#define PAGE_ALIGN_UP(x) \
  ((((uint64)(x)) + PAGE_MASK) & ~PAGE_MASK)  // Round up to a page boundary
#define PAGE_ALIGNED(x) \
  ((((uint64)(x)) & PAGE_MASK) == 0)  // True if aligned, False if not

// False while the MMU is off, when every pointer is a physical address; true
// once mmu_enable() has turned the MMU on, and the kernel reaches
// physical memory through the high map.
extern bool mmu_active;

// Converts a physical address into a pointer that the kernel can use: the
// address itself while the MMU is off, or its high-map address once it is on.
static inline uint64* mmu_pa_to_ptr(uint64 pa) {
  return (uint64*)(mmu_active ? KERNEL_PHYS_TO_VIRT(pa) : pa);
}

// Converts a kernel pointer (for example, one that kalloc() returned) back
// into a physical address.
static inline uint64 mmu_ptr_to_pa(uint64* va) {
  uint64 a = (uint64)va;
  return a >= KERNEL_VIRTUAL_BASE ? KERNEL_VIRT_TO_PHYS(a) : a;
}

#endif  // _MMU_MACROS_H

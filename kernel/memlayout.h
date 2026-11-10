// Raspberry Pi 3B (BCM2837 SoC) physical memory layout,
// as defined in official documentation:
// https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
//
// except for the following differences:
// - RPi 3 uses a peripheral base address of 0x3F00'0000 rather than
//   0x2000'0000

#ifndef _MEMLAYOUT_H
#define _MEMLAYOUT_H

// Location of the initial kernel stack in physical memory.
// Must be 16-byte aligned.
// #define INITIAL_KERNEL_STACK YOUR_CHOSEN_VALUE

// Peripherals are mapped starting at 0x3F00'0000, giving 1 MB for I/O memory
// Note that the documentation lists addresses starting at 0x7E00'0000. This is
// where the actual ARM hardware has these peripherals, but they are mapped into
// the SoC's address space at 0x3F00'0000
#define PERIPHERALS_BASE 0x3F000000
#define PERIPHERALS_LIMIT 0x3FFFFFFF

// "ARM local peripherals" are those peripherals that sit on the ARM CPU
// itself, rather than on the larger RPi SoC (e.g., the GPIO pins are an SoC
// peripheral, but the per-core timer interrupt routing is an ARM local
// peripheral). They are mapped starting at 0x4000'0000, right above the SoC
// peripherals, and are documented separately from them -- see Appendix B.
#define ARM_LOCAL_PERIPHERALS_BASE 0x40000000

// Project 4 adds one new region: a kernel stack for every process (Quest 1,
// Part B). Pick a place for NPROC of them that collides with nothing else,
// define it here (or in limits.h), and add it to your memory layout diagram:
//
// #define KSTACK_SIZE YOUR_CHOSEN_VALUE   // 16 KiB is plenty
// #define KSTACKS_BASE YOUR_CHOSEN_VALUE  // keep every stack top 16-byte aligned

// The virtual address where the kernel's "high map" starts once mmu_init
// has turned on the MMU. Physical address pa appears in the high map at
// virtual address KERNEL_VIRTUAL_BASE + pa.
//
// 0x8000'0000'0000 has only bit 47 set: it is the middle of the 48-bit range
// that TTBR0_EL1 translates (index 256 in the top-level page table). This
// leaves room in the high map for all of physical memory, well away from the
// low virtual addresses that processes use.
//
// Keep this a plain number (no ULL suffix): boot.S includes this file.
#define KERNEL_VIRTUAL_BASE 0x800000000000
#define KERNEL_PHYS_TO_VIRT(x) (((uint64)(x)) + KERNEL_VIRTUAL_BASE)
#define KERNEL_VIRT_TO_PHYS(x) (((uint64)(x)) - KERNEL_VIRTUAL_BASE)

// The pool of physical pages that kalloc() hands out. This must be a
// page-aligned region of RAM that collides with nothing else (including your
// process slots, until Project 5, Quest 4 removes them). Define it here,
// and add it to your memory layout diagram. For a 128 MiB pool:
//
// #define KALLOC_BASE YOUR_CHOSEN_VALUE   // e.g., 0x10000000 (256 MiB)
// #define KALLOC_LIMIT YOUR_CHOSEN_VALUE  // e.g., 0x18000000 (384 MiB)

// Trampoline to user-reachable functions (like yield or printf).
// This is essentially an array of function pointers at a known location, so
// that programs can use these functions as a library.
//
// The pointers get initialized in `kernel_main` (kernel.c) to point to the
// actual machine code. User programs can then call kernel functions by simply
// following those pointers (even though they weren't compiled together with
// the kernel).
// TODO: you'll need to change F_BASE in Quest 3 of Project 1.
#define F_BASE 0x0
#define F_VPRINTF (F_BASE - sizeof(void*))

// Project 3 adds several more of these: a way for a process to read a
// character, to find its canvas, and so on. Add them here as you
// go, following the same pattern (successive pointer-sized slots at
// *decreasing* addresses from F_BASE). Anything you name `F_...` is copied
// into user/u_klib.h automatically by user/gen_klib_header.py, so user
// programs can see it without you editing a generated file.
//
// #define F_GETC (F_BASE - 3 * sizeof(void*))
// ...

// Location of the framebuffer info struct (`struct fb_info`, which you will
// define in kernel/drivers/fb.h). Your kernel fills this in at boot and user
// programs read it to find out where the pixels are and how they are laid
// out -- the same fixed-address trick as F_BASE above, but carrying a struct
// rather than a function pointer.
//
// NOTE: this one is *not* named `F_...`, so it is not copied into
// user/u_klib.h for you. User programs need their own matching definition;
// see user/lib/u_fb.h.
// #define FB_INFO_BASE YOUR_CHOSEN_VALUE

#endif  // _MEMLAYOUT_H

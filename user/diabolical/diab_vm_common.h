// Constants for diabolical programs using virtual memory

#ifndef _DIAB_VM_COMMON_H
#define _DIAB_VM_COMMON_H

#include "diab_delay.h"
#include "u_common.h"
#include "u_syscalls.h"

// Start address to use for the kernel during tests
// Some tests write directly to kernel memory at this address
// (what should happen in these cases?)
#define KERNEL_BASE_ADDR 0x80'000

// The address where every program is linked: the `. =` at the start of
// SECTIONS in user/procs.ld.
#define DIAB_IMAGE_BASE 0x10000UL

// The memory that diab_scan() searches: all of RAM below 960 MiB. (Above it
// are the GPU's memory and the device registers, which it leaves alone.)
#define DIAB_SCAN_LIMIT 0x3C000000UL

// Searches memory outward from `start`: first the 8-byte word at `start`, then
// the words 8 bytes above and below it, then 16 bytes, and so on, until
// match() returns true for a word. Returns that word, or nullptr if no word
// below DIAB_SCAN_LIMIT matches.
static inline uint64* diab_scan(uint64 start, bool (*match)(uint64* p)) {
  start &= ~7UL;
  for (uint64 d = 0; d < DIAB_SCAN_LIMIT; d += 8) {
    if (start + d < DIAB_SCAN_LIMIT && match((uint64*)(start + d))) {
      return (uint64*)(start + d);
    }
    if (d != 0 && d <= start && match((uint64*)(start - d))) {
      return (uint64*)(start - d);
    }
  }
  return nullptr;
}

// diab_vm07_child keeps its money in an account. The account's `self` field
// points to the account itself, so a copy of the account that sits where the
// program's file says it should be is easy to tell apart from the real one.
struct diab_account {
  uint64 magic;
  struct diab_account* self;
  volatile int64 balance;
};
#define DIAB_ACCOUNT_MAGIC 0xACC0'D1AB'07AC'C007UL

#endif  // _DIAB_VM_COMMON_H

#include "vm/mmu_macros.h"

// Whether the MMU is on (see vm/mmu_macros.h). It stays false until
// mmu_enable() turns the MMU on.
bool mmu_active = false;

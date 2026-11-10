// PROVIDED: the synthetic page table (see mmu_test_synth.h). Do not change
// this file. It deliberately uses none of your page table code, and none of
// the macros in vm/mmu_regs.h, so that a bug there cannot hide a bug in your
// walk.

#include "vm/mmu_test_synth.h"

#include "memlayout.h"
#include "types.h"
#include "utils.h"  // memset()

// Descriptor bits (see Appendix A of the handout).
#define SYNTH_VALID (1ULL << 0)
#define SYNTH_TYPE (1ULL << 1)  // table (upper levels), or page (last level)
#define SYNTH_ATTRINDX(i) ((uint64)(i) << 2)
#define SYNTH_AP_EL0 (1ULL << 6)
#define SYNTH_AP_RO (1ULL << 7)
#define SYNTH_SH_INNER (3ULL << 8)
#define SYNTH_AF (1ULL << 10)
#define SYNTH_NG (1ULL << 11)
#define SYNTH_PXN (1ULL << 53)
#define SYNTH_UXN (1ULL << 54)
#define SYNTH_SW(n) (1ULL << (55 + (n)))  // software bits 55 to 58

// Attributes for last-level descriptors.
#define SYNTH_USER_DATA \
  (SYNTH_AF | SYNTH_SH_INNER | SYNTH_AP_EL0 | SYNTH_PXN | SYNTH_UXN)
#define SYNTH_USER_CODE \
  (SYNTH_AF | SYNTH_SH_INNER | SYNTH_AP_EL0 | SYNTH_AP_RO | SYNTH_PXN)
#define SYNTH_KERNEL (SYNTH_AF | SYNTH_SH_INNER | SYNTH_UXN)
#define SYNTH_DEVICE \
  (SYNTH_AF | SYNTH_SH_INNER | SYNTH_ATTRINDX(1) | SYNTH_PXN | SYNTH_UXN)

// A last-level page descriptor that maps the page at physical address pa.
#define SYNTH_PAGE(pa, attr) \
  ((uint64)(pa) | (attr) | SYNTH_TYPE | SYNTH_VALID)

#define SYNTH_ENTRIES 512  // descriptors per table
#define SYNTH_NTABLES 19   // tables in the synthetic page table

static uint64 synth_tables[SYNTH_NTABLES][SYNTH_ENTRIES]
    __attribute__((aligned(4096)));

// The test cases. The comment above each one gives the indices into the
// top-level, 2nd-level, 3rd-level, and last-level tables, and the offset.
static const struct synth_pt_case synth_cases[] = {
    // [0, 0, 2, 1] + 0x234
    {"a page at a low address", 0x401234, true,
     SYNTH_PAGE(0x0A5A5000, SYNTH_USER_DATA), 0x0A5A5234},
    // [0, 0, 2, 511] + 0xFFF
    {"the last byte of the last page in a last-level table", 0x5FFFFF, true,
     SYNTH_PAGE(0x01234000, SYNTH_USER_CODE), 0x01234FFF},
    // [0, 0, 3, 0] + 0x000: the next byte after the one above, but in another
    // last-level table, and in an unrelated physical page
    {"the first byte of the next last-level table", 0x600000, true,
     SYNTH_PAGE(0x0BEEF000, SYNTH_USER_DATA | SYNTH_NG | SYNTH_SW(0)),
     0x0BEEF000},
    // [0, 1, 0, 7] + 0x008
    {"a device page, under another 2nd-level descriptor", 0x40007008, true,
     SYNTH_PAGE(0x3F200000, SYNTH_DEVICE), 0x3F200008},
    // [511, 511, 511, 511] + 0xABC, mapped to a physical address with bit 47
    // set
    {"the highest page that TTBR0 translates", 0xFFFFFFFFFABC, true,
     SYNTH_PAGE(0x812345678000, SYNTH_KERNEL), 0x812345678ABC},
    // [256, 0, 0, 129] + 0x234: KERNEL_VIRTUAL_BASE + 0x81234
    {"a kernel address in the high map", 0x800000081234, true,
     SYNTH_PAGE(0x00081000, SYNTH_KERNEL), 0x00081234},
    // [1, 2, 3, 4] + 0x567
    {"indices 1, 2, 3, 4 from the top level down", 0x008080604567, true,
     SYNTH_PAGE(0x11111000, SYNTH_USER_DATA), 0x11111567},
    // [4, 3, 2, 1] + 0x765
    {"indices 4, 3, 2, 1 from the top level down", 0x0200C0401765, true,
     SYNTH_PAGE(0x22222000,
                SYNTH_USER_DATA | SYNTH_SW(1) | SYNTH_SW(2) | SYNTH_SW(3)),
     0x22222765},

    // [2, 0, 0, 0]
    {"an invalid (zero) top-level descriptor", 0x010000000000, false, 0, 0},
    // [3, 0, 2, 1] + 0x234: top-level descriptor 3 has its valid bit clear,
    // but otherwise looks like a table descriptor
    {"an invalid top-level descriptor that is not zero", 0x018000401234,
     false, 0, 0},
    // [5, 0, 2, 1] + 0x234: top-level descriptor 5 has bits 1:0 = 0b01 (its
    // type bit is clear). That is invalid in the top-level table, even though
    // the descriptor points to the same 2nd-level table as top-level
    // descriptor 0, under which the first case above is mapped.
    {"a top-level descriptor with bits 1:0 = 0b01", 0x028000401234, false, 0,
     0},
    // [0, 5, 0, 0]
    {"an invalid (zero) 2nd-level descriptor", 0x140000000, false, 0, 0},
    // [0, 0, 4, 0]
    {"an invalid (zero) 3rd-level descriptor", 0x800000, false, 0, 0},
    // [0, 0, 2, 2] + 0x010
    {"an invalid (zero) last-level descriptor", 0x402010, false, 0, 0},
    // [0, 0, 2, 3] + 0x010: last-level descriptor 3 has its valid bit clear,
    // but otherwise looks like a page descriptor
    {"an invalid last-level descriptor that is not zero", 0x403010, false, 0,
     0},
    // [0, 0, 2, 4] + 0x010: last-level descriptor 4 has bits 1:0 = 0b01 (its
    // type bit is clear), which is invalid in a last-level table
    {"a last-level descriptor with bits 1:0 = 0b01", 0x404010, false, 0, 0},
    // The low 48 bits are those of the first case above, but bit 48 is set
    {"an address outside the TTBR0 range (bit 48 set)", 0x0001000000401234,
     false, 0, 0},
};

// Returns the physical address of a table. Before the MMU is on, the kernel
// runs at its physical addresses; afterwards, it may run in the high map.
static uint64 synth_pa(const uint64* table) {
  uint64 a = (uint64)table;
  return a >= KERNEL_VIRTUAL_BASE ? KERNEL_VIRT_TO_PHYS(a) : a;
}

// Returns a table descriptor that points to next.
static uint64 synth_table(const uint64* next) {
  return synth_pa(next) | SYNTH_TYPE | SYNTH_VALID;
}

uint64* mmu_test_generate_synth_pt(const struct synth_pt_case** cases,
                                   uint64* ncases) {
  memset(synth_tables, 0, sizeof(synth_tables));

  // Each table is named after the indices that lead to it: t0_0_2 is the
  // last-level table that top[0], then t0[0], then t0_0[2] point to.
  uint64* top = synth_tables[0];
  uint64* t0 = synth_tables[1];
  uint64* t0_0 = synth_tables[2];
  uint64* t0_0_2 = synth_tables[3];
  uint64* t0_0_3 = synth_tables[4];
  uint64* t0_1 = synth_tables[5];
  uint64* t0_1_0 = synth_tables[6];
  uint64* t511 = synth_tables[7];
  uint64* t511_511 = synth_tables[8];
  uint64* t511_511_511 = synth_tables[9];
  uint64* t256 = synth_tables[10];
  uint64* t256_0 = synth_tables[11];
  uint64* t256_0_0 = synth_tables[12];
  uint64* t1 = synth_tables[13];
  uint64* t1_2 = synth_tables[14];
  uint64* t1_2_3 = synth_tables[15];
  uint64* t4 = synth_tables[16];
  uint64* t4_3 = synth_tables[17];
  uint64* t4_3_2 = synth_tables[18];

  // Top-level table
  top[0] = synth_table(t0);
  top[1] = synth_table(t1);
  top[3] = synth_table(t0) & ~SYNTH_VALID;  // invalid, but not zero
  top[4] = synth_table(t4);
  top[5] = synth_table(t0) & ~SYNTH_TYPE;  // 0b01: invalid at this level
  top[256] = synth_table(t256);
  top[511] = synth_table(t511);

  // Under top[0]
  t0[0] = synth_table(t0_0);
  t0[1] = synth_table(t0_1);
  t0_0[2] = synth_table(t0_0_2);
  t0_0[3] = synth_table(t0_0_3);
  t0_0_2[1] = synth_cases[0].desc;
  t0_0_2[3] =
      SYNTH_PAGE(0x00555000, SYNTH_USER_DATA) & ~SYNTH_VALID;  // invalid
  t0_0_2[4] = SYNTH_PAGE(0x00666000, SYNTH_USER_DATA) &
              ~SYNTH_TYPE;  // 0b01: invalid at this level
  t0_0_2[511] = synth_cases[1].desc;
  t0_0_3[0] = synth_cases[2].desc;
  t0_1[0] = synth_table(t0_1_0);
  t0_1_0[7] = synth_cases[3].desc;

  // Under top[511]
  t511[511] = synth_table(t511_511);
  t511_511[511] = synth_table(t511_511_511);
  t511_511_511[511] = synth_cases[4].desc;

  // Under top[256]
  t256[0] = synth_table(t256_0);
  t256_0[0] = synth_table(t256_0_0);
  t256_0_0[129] = synth_cases[5].desc;

  // Under top[1]
  t1[2] = synth_table(t1_2);
  t1_2[3] = synth_table(t1_2_3);
  t1_2_3[4] = synth_cases[6].desc;

  // Under top[4]
  t4[3] = synth_table(t4_3);
  t4_3[2] = synth_table(t4_3_2);
  t4_3_2[1] = synth_cases[7].desc;

  *cases = synth_cases;
  *ncases = sizeof(synth_cases) / sizeof(synth_cases[0]);
  return top;
}

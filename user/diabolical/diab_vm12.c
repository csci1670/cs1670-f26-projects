// diab_vm12.c:  Virtual memory diabolical program 12

#include "diab_vm_common.h"
#include "u_common.h"

static void* memset(void* src, int c, uint64 n);

#define ARRAY_SIZE (1UL * 1024UL * 1024UL * 1024UL)  // 1 GB
char data[ARRAY_SIZE];

int main() {
  diab_delay();  // let the rest of the system get going first

  memset(data, 0, ARRAY_SIZE);  // Clear the data array

  printf("diab_vm12:  done!\r\n");

  return 0;
}

// Helper to clear some memory
static void* memset(void* src, int c, uint64 n) {
  char* s = src;

  for (uint64 i = 0; i < n; ++i) {
    s[i] = c;
  }

  return src;
}

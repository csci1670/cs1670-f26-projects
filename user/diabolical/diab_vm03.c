// diab_vm03.c:  Virtual memory diabolical program 3

#include "diab_vm_common.h"
#include "u_common.h"
#include "u_syscalls.h"

#define KERNEL_ADDR ((void*)KERNEL_BASE_ADDR)

int main(void) {
  diab_delay();  // let the rest of the system get going first

  printf("writing from %p:\r\n", KERNEL_ADDR);

  int64 n = write(1, KERNEL_ADDR, 32);

  printf("\r\nwrite returned %d\r\n", (int)n);

  printf("diab_vm03: done!\r\n");

  return 0;
}

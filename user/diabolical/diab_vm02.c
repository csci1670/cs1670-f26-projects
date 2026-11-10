// diab_vm02.c:  Virtual memory diabolical program 2

#include "diab_vm_common.h"
#include "u_common.h"
#include "u_syscalls.h"

int main(void) {
  diab_delay();  // let the rest of the system get going first

  unsigned long* arr = (unsigned long*)KERNEL_BASE_ADDR;

  for (int i = 0; i < 512; i++) {
    arr[i] = 0;
    printf("set %p to zero!\r\n", &arr[i]);
  }

  printf("diab_vm02: done!\r\n");

  return 0;
}

// diab_vm01.c:  Virtual memory diabolical program 1

#include "diab_vm_common.h"
#include "u_common.h"
#include "u_syscalls.h"

int main(void) {
  diab_delay();  // let the rest of the system get going first

  unsigned long* arr = (unsigned long*)KERNEL_BASE_ADDR;

  for (int i = 0; i < 512; i++) {
    unsigned long value = arr[i];
    printf("%p has value %p\r\n", (void*)&arr[i], (void*)value);
  }

  printf("diab_vm01: done!\r\n");

  return 0;
}

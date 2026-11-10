// diab_vm04.c:  Virtual memory diabolical program 4

#include "diab_vm_common.h"

int main(void) {
  diab_delay();  // let the rest of the system get going first

  volatile unsigned long* p = nullptr;  // nullptr is address 0x0

  *p = 0xDEADBEEF;

  printf("wrote %p to address 0\r\n", (void*)*p);

  printf("diab_vm04: done!\r\n");

  return 0;
}

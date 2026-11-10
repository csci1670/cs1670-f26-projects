// diab_vm10.c:  Virtual memory diabolical program 10

#include "diab_vm_common.h"
#include "u_common.h"
#include "u_syscalls.h"

int main() {
  diab_delay();  // let the rest of the system get going first

  // let's reboot the kernel!
  void (*ptr)(void) = (void (*)(void))0x80000;
  ptr();

  printf("diab_vm10:  done!\r\n");  // should never be reached -- why?

  return 0;
}

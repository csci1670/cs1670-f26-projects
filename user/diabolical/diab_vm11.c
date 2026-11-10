// diab_vm11.c:  Virtual memory diabolical program 11

#include "diab_vm_common.h"
#include "u_common.h"
#include "u_syscalls.h"

int main() {
  diab_delay();  // let the rest of the system get going first

  int loop[2] = {0xd503201f, 0x17ffffff};  // machine code for while(1) {}
  void (*ptr)(void) =
      (void (*)(void))loop;  // make a function pointer to that code
  ptr();

  printf("diab_vm11:  done!\r\n");  // should never be reached -- why?

  return 0;
}

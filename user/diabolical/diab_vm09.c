// diab_vm09.c:  Virtual memory diabolical program 9

#include "diab_vm_common.h"
#include "u_common.h"
#include "u_syscalls.h"

char* secret_password = "My password is:  hello_world123";

int main() {
  diab_delay();  // let the rest of the system get going first

  int child = spawn("diab_vm09_child", 0, 1);

  wait(child);

  printf("diab_vm09:  done!\r\n");

  return 0;
}

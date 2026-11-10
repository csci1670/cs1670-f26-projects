// diab_vm08.c:  Virtual memory diabolical program 8

#include "diab_vm_common.h"

int main(void) {
  diab_delay();  // let the rest of the system get going first

  // Spawn two processes
  int a = spawn("diab_vm08_child", 0,
                1);  // see diab_vm08_child.c (interesting code is here)
  int b = spawn("diab_vm08_child", 0,
                1);  // see diab_vm08_child.c (interesting code is here)

  wait(a);
  wait(b);

  printf("diab_vm08:  done!\r\n");

  return 0;
}

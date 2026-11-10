// diab_vm07.c:  Virtual memory diabolical program 7

#include "diab_vm_common.h"

// Is p the account of a running diab_vm07_child?
static bool is_account(uint64* p) {
  struct diab_account* a = (struct diab_account*)p;
  return a->magic == DIAB_ACCOUNT_MAGIC && a->self == a;
}

int main(void) {
  // Start a process that has some money in its account.
  int child = spawn("diab_vm07_child", 0, 1);
  diab_delay();  // let the rest of the system get going first

  // Do some diabolical banking!
  int here;
  struct diab_account* a =
      (struct diab_account*)diab_scan((uint64)&here, is_account);
  if (a == nullptr) {
    printf("diab_vm07: found no account\r\n");
  } else {
    printf("diab_vm07: found an account with %d in it, at %p\r\n",
           (int)a->balance, a);
    a->balance = 0;
  }

  wait(child);
  printf("diab_vm07: done!\r\n");
  return 0;
}

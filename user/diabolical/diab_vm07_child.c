// diab_vm07_child.c:  A process with some money, for diab_vm07

#include "diab_vm_common.h"

struct diab_account account = {DIAB_ACCOUNT_MAGIC, &account, 100};

int main(void) {
  printf("diab_vm07_child: I have %d in my account\r\n", (int)account.balance);

  // Wait a while, without touching the account.
  for (int i = 0; i < 3; i++) {
    diab_delay();
  }

  printf("diab_vm07_child: I now have %d in my account\r\n",
         (int)account.balance);
  return 0;
}

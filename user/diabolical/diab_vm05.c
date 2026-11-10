// diab_vm05.c:  Virtual memory diabolical program 5

#include "diab_vm_common.h"

#pragma GCC diagnostic ignored \
    "-Winfinite-recursion"  // suppress compiler warning

#define FRAME_SIZE 4096

int recurse(int depth) {
  volatile char frame[FRAME_SIZE];

  // Write something to each end of the array
  frame[0] = (char)depth;
  frame[FRAME_SIZE - 1] = (char)depth;

  printf("depth %d\r\n", depth);

  return frame[0] + recurse(depth + 1);
}

int main(void) {
  diab_delay();  // let the rest of the system get going first

  recurse(0);

  printf("diab_vm05: done!\r\n");  // Will never reach this -- why?

  return 0;
}

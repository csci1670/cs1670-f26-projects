// diab_vm08_child.c:  Worker process for diab_vm08

#include "diab_vm_common.h"
#include "u_common.h"
#include "u_syscalls.h"

// Where this program starts (see user/procs.ld).
extern char __image_start[];

// We want to store our favorite number here
int fav_number;

int main(void) {
  // The address that fav_number has in this program's file, before the
  // loader puts the program in memory: the same in every copy of the program.
  // (assume that this address is not used by any other process)
  int* fav_number_address =
      (int*)((uint64)&fav_number - (uint64)__image_start + DIAB_IMAGE_BASE);

  int my_pid = getpid();
  *fav_number_address = my_pid;  // store our favorite number (which is our PID)

  // Be *really* sure our address has our number
  for (int i = 0; i < 1000000; i++) {
    if (i % 1000 == 0) {
      yield();  // give the other worker a turn, too
    }

    int found = *fav_number_address;

    if (found != my_pid) {
      printf("diab_vm08, worker %d encountered error:  expected %d, got %d\r\n",
             my_pid, my_pid, found);
      exit(-1);
    }
  }

  // Without virtual memory, only one of the workers gets here -- why?
  printf("diab_vm08:  worker %d done!\r\n", getpid());
  return 0;
}

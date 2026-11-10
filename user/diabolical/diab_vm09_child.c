// diab_vm09_child.c:  Worker process for diab_vm09

#include "diab_vm_common.h"
#include "u_common.h"
#include "u_syscalls.h"

#define SCAN_START 0x0UL
#define SCAN_END 0x3F000000UL

// Print a dot every 64 MB, so that we can see the scan making progress
#define PROGRESS_INTERVAL (64UL * 1024UL * 1024UL)

// The longest match we print (in case there is no NUL after it)
#define MAX_PRINT 128

const char* pattern = "My password is:";

int main(void) {
  int len = 0;
  while (pattern[len] != '\0') {
    len++;
  }

  printf("diab_vm09_child:  scanning memory ...\r\n");

  for (uint64 addr = SCAN_START; addr + len <= SCAN_END; addr++) {
    if (addr % PROGRESS_INTERVAL == 0) {
      printf(".");
    }

    const char* p = (const char*)addr;
    if (p == pattern) {
      continue;  // our own copy of the pattern
    }

    // Does the pattern start here?
    int i = 0;
    while (i < len && p[i] == pattern[i]) {
      i++;
    }
    if (i < len) {
      continue;
    }

    // Found!  Print up to the null terminator in the found string
    printf("\r\nfound at 0x%x:  ", (unsigned int)addr);
    for (int j = 0; j < MAX_PRINT && p[j] != '\0'; j++) {
      printf("%c", p[j]);
    }
    printf("\r\n");
  }

  printf("\r\ndiab_vm09_child:  done\r\n");
  return 0;
}

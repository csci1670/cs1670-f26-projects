// diab_vm06.c:  Virtual memory diabolical program 6

#include "diab_vm_common.h"

#define UART_BASE_ADDR 0x3F201000
#define UART_CR_ADDR (UART_BASE_ADDR + 0x30)

int main(void) {
  volatile unsigned int* reg = (volatile unsigned int*)(UART_CR_ADDR);

  unsigned long value = (unsigned long)*reg;
  printf("UART control register holds %p\r\n", (void*)value);

  printf("I think I will make it not that!\r\n");
  *reg = 0;

  // Will never print these -- why?
  printf("Hello world!\r\n");
  printf("diab_vm06: done!\r\n");

  return 0;
}

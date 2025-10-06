#include "long.h"
#include "printf.h"
#include <stdint.h>
int main(void) {
  char msg[] = "Hello World from Stage2!";
  const char COLOR = 11;
  char *vmem = (char *)0xb8000;
  for (int i = 0; i < sizeof(msg) - 1; i++) {
    *(vmem++) = msg[i];
    *(vmem++) = COLOR;
  }

  putc_init_cursor();
  printf("\nThis is a printf test!\n");

  if (x64_supported()) {
    printf("This CPU supports Long Mode!\n");
  }

  asm volatile("loop: hlt; jmp loop");
}

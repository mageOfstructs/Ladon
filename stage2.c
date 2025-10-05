#include <stdint.h>
int main(void) {
  char msg[] = "Hello World from Stage2!";
  const char COLOR = 11;
  char *vmem = (char *)0xb8000;
  for (int i = 0; i < sizeof(msg) - 1; i++) {
    *(vmem++) = msg[i];
    *(vmem++) = COLOR;
  }
}

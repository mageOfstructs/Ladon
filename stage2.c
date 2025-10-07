#include "long.h"
#include "printf.h"
#include "ata.h"
#include <stdint.h>

typedef struct mmape {
  uint64_t base;
  uint64_t length;
  uint32_t type;
  uint32_t ext_attrs;
} mmape_t;

int main(void) {
  char msg[] = "Hello World from Stage2!";
  const char COLOR = 11;
  char *vmem = (char *)0xb8000;
  for (int i = 0; i < sizeof(msg) - 1; i++) {
    *(vmem++) = msg[i];
    *(vmem++) = COLOR;
  }

  putc_init_cursor();
  printf("\nThis is a printf test %d %p!\n", 43, (void *)0x12345);

  mmape_t *entries = (mmape_t *)0x502;
  uint32_t totalmem = 0, usram;
  for (uint16_t i = 0; i < *((uint16_t *)0x500); i += 24) {
    if (!entries->length)
      continue;
    totalmem += entries->length;
    if (entries->type == 1)
      usram += entries->length;

    printf("%p-", entries->base);
    printf("%p", (void *)(entries->base + entries->length));
    printf(" (%l bytes) Type: %d\n", entries->length, entries->type);
    entries++;
  }

  printf("\nTotal RAM: %lM\n", totalmem / 1024);
  printf("Usable RAM: %lM\n", usram / 1024);

  if (x64_supported()) {
    printf("This CPU supports Long Mode!\n");
  }

  uint16_t buf[256];
  if (identify(buf) == IDENTIFY_ATA) {
    printf("found ATA!\n");
    uint16_t bootsec[256];
    read_ata(true, 0, 512, bootsec);
    printf("BSSIG: %p\n", bootsec[255]);
    uint32_t *part_lba_start = &((uint8_t *)buf)[0x1BE + 0x8];
  }

  asm volatile("loop: hlt; jmp loop");
}

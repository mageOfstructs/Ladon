#include "elf.h"
#include "long.h"
#include "printf.h"
#include "ata.h"
#include "fs/ext2.h"
#include "serial.h"
#include <stddef.h>
#include <stdint.h>
#include "mmap.h"

int main(void) {
  init_serial();
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
  uint32_t entries_l = *((uint16_t *)0x500) / 24;
  uint32_t totalmem = 0, usram;
  for (uint16_t i = 0; i < entries_l; i++) {
    if (!entries[i].length)
      continue;
    totalmem += entries[i].length;
    if (entries[i].type == 1)
      usram += entries[i].length;

    printf("%p-", entries[i].base);
    printf("%p", (void *)(entries[i].base + entries[i].length));
    printf(" (%l bytes) Type: %d\n", entries[i].length, entries[i].type);
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
    uint8_t *buf_bytes = (uint8_t *)bootsec;
    uint32_t *part_lba_start = (uint32_t *)&buf_bytes[0x1BE + 0x8];
    uint32_t *part_lba_secs = (uint32_t *)&buf_bytes[0x1BE + 0xC];

    println("\nDetected partition at %p with %d sectors (%dM)", *part_lba_start,
            *part_lba_secs, *part_lba_secs * 512 / 1024 / 1024);
    init_fs(*part_lba_start);

    load_elf("test_elf", entries, entries_l);
  }

  asm volatile("loop: hlt; jmp loop");
}

#include "ata.h"
#include "avl.h"
#include "elf.h"
#include "fs/ext2.h"
#include "log.h"
#include "long.h"
#include "mmap.h"
#include "printf.h"
#include "serial.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

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
  uint32_t totalmem = 0, usram = 0;
  uint64_t last_section_end = 0;

  for (uint16_t i = 0; i < entries_l; i++) {
    if (!entries[i].length)
      continue;
    if (last_section_end > entries[i].base)
      warn("Overlapping section found!\n");
    if (last_section_end != entries[i].base)
      warn("Disjunct sections found!\n");

    totalmem += entries[i].length;
    if (entries[i].type == MMAP_FREE)
      usram += entries[i].length;

    last_section_end = entries[i].base + entries[i].length;

    printf("%p-%lx (%d bytes) Type: %d\n", (void *)entries[i].base,
           last_section_end, (int)entries[i].length, entries[i].type);
  }

  printf("\nTotal RAM: %dM\n", totalmem / 1024);
  printf("Usable RAM: %dM\n", usram / 1024);

  if (x64_supported()) {
    printf("This CPU supports Long Mode!\n");
  }

  // paging
  tree_node_t tree_buf[entries_l];
  tree_buf[0] = TREE_NODE_INIT;
  int first_free_space = get_next_free_space(entries, entries_l, 0);
  KASSERT(first_free_space != -1);
  printf("Found free space at %d\n", first_free_space);
  tree_buf[0].val = &entries[first_free_space];

  int next_free_space = first_free_space + 1, tree_bufi = 1;
  while ((next_free_space =
              get_next_free_space(entries, entries_l, next_free_space)) != -1) {
    printf("Found free space at %d\n", next_free_space);
    tree_buf[tree_bufi] = TREE_NODE_INIT;
    tree_buf[tree_bufi].val = &entries[next_free_space];
    tree_bufi++;
    append_ordered(tree_buf, &tree_buf[tree_bufi], cmp_mmape_len);
    next_free_space++;
  }
  dbg_tree(tree_buf, 0, tostr_mmape_len);
  asm volatile("loop: hlt; jmp loop");

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
}

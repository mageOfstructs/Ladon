#include "elf.h"
#include "log.h"
#include "fs/ext2.h"
#include "printf.h"
#include <stdbool.h>
#include <stdint.h>

extern fs_ext2_ctx_t FS_GLOBAL_CTX;
const static uint32_t ELF_MAGIC = (0x7F << 24) | ('E' << 16) | ('L' << 8) | 'F';
const static uint32_t ELF_MAGIC_REV =
    ('F' << 24) | ('L' << 16) | ('E' << 8) | 0x7F;

static inline Elf32_Shdr *elf_sheader(Elf32_Ehdr *hdr) {
  return (Elf32_Shdr *)((int)hdr + hdr->e_shoff);
}

static inline Elf32_Shdr *elf_section(Elf32_Ehdr *hdr, int idx) {
  return &elf_sheader(hdr)[idx];
}
static inline char *lookup_string(Elf32_Ehdr *hdr, uint32_t off) {
  return (char *)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset + off;
}

int find_memory_hole(mmape_t *entries, uint32_t entries_l, uint32_t elf_sz) {
  for (int i = entries_l - 1; i > -1; i--) {
    if (!entries[i].length || entries[i].type != 1)
      continue;
    if (entries[i].length >= elf_sz) {
      return i;
    }
  }
  return -1;
}

static int load_elf_ext2(char *path, mmape_t *mmap_entries,
                         uint32_t mmap_entries_l) {
  inode_t i;
  if (traverse(path, NULL, &i))
    return -1;
  int ret = find_memory_hole(mmap_entries, mmap_entries_l, i.lsize);
  if (ret < 0) {
    return -2;
  }
  void *buf = (void *)mmap_entries[ret].base;
  println("Loading ELF at %p", buf);
  int read_ret = read_from_inode_full(i, buf);
  if (!read_ret) {
    return -3;
  }

  return ret;
}

int ck_elf_magic(Elf32_Ehdr *hdr) {
  uint32_t magic = *((uint32_t *)hdr->e_ident);
  if (magic == ELF_MAGIC)
    return CK_ELF_MAGIC_BE;
  if (magic == ELF_MAGIC_REV)
    return CK_ELF_MAGIC_LE;
  return CK_ELF_MAGIC_INVALID;
}

void dump_sht(Elf32_Ehdr *hdr) {
  if (hdr->e_shstrndx == SHN_UNDEF) {
    warn("String table missing\n");
  }
  for (int i = 0; i < hdr->e_shnum; i++) {
    Elf32_Shdr *sh = elf_section(hdr, i);
    switch (sh->sh_type) {
    case SHT_NULL:
      printf("Null");
      break;
    case SHT_PROGBITS:
      printf("Progbits");
      break;
    case SHT_SYMTAB:
      printf("Symbol Table");
      break;
    case SHT_STRTAB:
      printf("String Table");
      break;
    case SHT_RELA:
      printf("Relocatable Section A");
      break;
    case SHT_NOBITS:
      printf("NOBITS section");
      break;
    case SHT_REL:
      printf("Relocatable Section");
      break;
    default:
      printf("Unknown Type");
    }

    printf(" (%d bytes) at %p", sh->sh_size, (void *)sh->sh_offset);
    if (hdr->e_shstrndx != SHN_UNDEF && sh->sh_name != SHN_UNDEF) {
      printf(" '%s'", lookup_string(hdr, sh->sh_name));
    }
    printf("\n");
  }
}

int load_elf(char *path, mmape_t *mmap_entries, uint32_t mmap_entries_l) {
  int ret = load_elf_ext2(path, mmap_entries, mmap_entries_l);
  if (ret < 0)
    return ret;
  uint8_t *elf_buf = (uint8_t *)mmap_entries[ret].base;
  printf("Loaded ELF!\n");
  Elf32_Ehdr *hdr = (Elf32_Ehdr *)elf_buf;

  int endianness = ck_elf_magic(hdr);
  if (endianness == CK_ELF_MAGIC_INVALID) {
    err("Invalid ELF header magic!");
    return 1;
  }
  if (endianness == CK_ELF_MAGIC_BE) {
    err("System does not support Big Endian ELFs!");
    return 2;
  }
  if (hdr->e_ident[EI_CLASS] != ELFCLASS32) {
    err("Only x86_32 elfs supported!");
    return 3;
  }
  // if (hdr->e_type != EM_386) {
  //   err("ELF not of the x86 type!");
  //   return 4;
  // }
  if (hdr->e_version != EV_CURRENT) {
    warn("Unknown ELF Version, trusting that it is backwards compatible with "
         "Version 1...");
  }
  if (!(hdr->e_type == ET_EXEC || hdr->e_type == ET_REL)) {
    err("Unknown ELF type: %d", hdr->e_type);
    return 5;
  }

  dump_sht(hdr);

  return 0;
}

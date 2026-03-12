#ifndef VM_H
#define VM_H

#include <stddef.h>
#include <stdint.h>

#include "mmap.h"

typedef struct pg_dire pg_dire_t;
typedef struct pt_entry pt_entry_t;

struct pg_dire {
    uint16_t flags : 12;
    uint32_t pt_addr : 20;
};

struct pt_entry {
    uint16_t flags : 12;
    uint32_t phys_addr : 20;
};

enum vm_flags {
    PRESENT = 1,
    RW = 1 << 1,
    USR = 1 << 2,
    PWT = 1 << 3,
    PCD = 1 << 4,
    ACCESSED = 1 << 5,
    PT_DIRTY = 1 << 6,
    PT_PAT = 1 << 7,
    PT_GLOBAL = 1 << 8,
};

void init_vm(mmape_t *entries, size_t entries_l);
void vm_alloc(size_t sz);
void vm_alloca(void *vmem_start, size_t sz);
void vm_map(void *phys_mem, void *virt_mem, size_t sz);

#endif

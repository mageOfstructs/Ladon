#ifndef VM_H
#define VM_H

#include "math.h"
#include "mmap.h"
#include <stdbool.h>
#include <stdint.h>

#define PG_SIZE 4096
#define TBL_LEN 1024

#define PD_I(vaddr) (vaddr >> 22)
#define PT_I(vaddr) ((vaddr >> 12) & 0x3FF) // preserve lower 10 bits
#define PGOFF(vaddr) (vaddr & 0xFFF)        // preserve lower 12 bits
#define PG(vaddr) (vaddr & ~0x3FF)

typedef uint32_t vm_rptr; // Raw pointer
typedef vm_rptr vm_int;
typedef vm_rptr vm_rent; // raw entry

#define GET_FLAG(ent, flag) ((ent >> flag) & 1)
#define SET_FLAG(ent, flag)                                                    \
  do {                                                                         \
    *((vm_rent *)ent) |= 1 << flag;                                            \
  } while (0);
#define IS_PRESENT(ent) GET_FLAG(ent, 0)

typedef struct pde {
  uint8_t flags;
  uint8_t avl : 4;
  uint32_t addr : 20;
} pde_t;

typedef struct pde_ps {
  uint8_t flags;
  uint8_t g : 1;
  uint8_t avl : 3;
  uint8_t pat : 1;
  uint8_t addr_l;
  uint8_t rsvd : 1;
  uint8_t addr_h : 7;
} pde_ps_t;

enum PDE_FLAGS {
  PD_PRESENT = 0,
  PD_RW = 1,
  PD_US = 2,
  PD_PWT = 3,
  PD_PCD = 4,
  PD_ACCESSED = 5,
  PD_AVL1 = 6,
  PD_PS = 7
};

enum PTE_FLAGS {
  PT_PRESENT = 0,
  PT_RW = 1,
  PT_US = 2,
  PT_PWT = 3,
  PT_PCD = 4,
  PT_ACCESSED = 5,
  PT_DIRTY = 6,
  PT_PAT = 7
};

typedef struct pte {
  uint8_t flags;
  uint8_t g : 1;
  uint8_t avl : 3;
  uint32_t addr : 20;
} pte_t;

enum setup_addrsp_ret { SA_OK = 0, SA_NOSPACE = 1 };

#define PDE_INIT (1)
#define PTE_INIT PDE_INIT
#define PDE_PS_INIT (129)

#endif // !VM_H

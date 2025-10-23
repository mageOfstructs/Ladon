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
  PRESENT = 0,
  RW = 1,
  US = 2,
  PWT = 3,
  PCD = 4,
  ACCESSED = 5,
  AVL1 = 6,
  PS = 7
};

enum setup_addrsp_ret { SA_OK = 0, SA_NOSPACE = 1 };

#define PDE_INIT (1)
#define PTE_INIT PDE_INIT
#define PDE_PS_INIT (129)

#endif // !VM_H

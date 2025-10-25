#include "vm.h"
#include <stdint.h>

void enable_paging(pde_t *pd) {
  asm volatile("mov eax, %0\n\t"
               "mov cr3, eax\n\t"
               "mov eax, cr0\n\t"
               "or eax, 0x80000001\n\t"
               "mov cr0, eax" ::"m"(pd)
               : "eax");
}

int identity_map(pde_t *pd, vm_rptr start_identity, vm_rptr end_identity,
                 bool rw) {
  vm_int needed_pages = ceild(end_identity - start_identity, PG_SIZE);

  vm_int pd_i_start = PD_I(start_identity);
  vm_rent *rpd = (vm_rent *)pd; // yeah just alias the fuck out of the pagedir
  pde_ps_t *pd_ps = (pde_ps_t *)pd;

  vm_rptr cur_page = PG(start_identity);
  int i = 0;
  for (; i < (end_identity - start_identity) / (PG_SIZE * TBL_LEN); i++) {
    rpd[pd_i_start + i] = PDE_PS_INIT;
    if (rw)
      SET_FLAG(&rpd[pd_i_start + i], PD_RW);

    pd_ps[pd_i_start + i].addr_l = cur_page >> 24;
    pd_ps[pd_i_start + i].addr_h = (cur_page >> 12) & 0x1FF;

    cur_page += PG_SIZE;
  }

  needed_pages -= i * TBL_LEN;
  for (i = 0; i < needed_pages; i++) {
  }

  return SA_OK;
}

#include "vm.h"
#include "alloc.h"

static pg_dire_t PAGE_DIR[1024];

void init_vm(mmape_t *entries, size_t entries_l) {
    mmape_t *maxe = entries;
    for (int i = 0; i < entries_l; i++) {
        if (maxe->length < entries[i].length) {
            maxe = &entries[i];
        }
    }
}

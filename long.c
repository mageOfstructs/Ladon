#include "long.h"
#include <stdint.h>

bool x64_supported() {
  uint32_t res;
  asm volatile("mov eax, %1\n\t"
               "cpuid\n\t"
               "mov %0, edx"
               : "=r"(res)
               : "i"(0x80000001));
  return (res >> 29) & 1;
}

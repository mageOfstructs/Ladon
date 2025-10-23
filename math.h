#ifndef MATH_H
#define MATH_H
#include <stdint.h>
#define ceild(divid, divis) (divid / divis + (divid % divis != 0 ? 1 : 0))
uint32_t max(uint32_t n1, uint32_t n2);
uint32_t min(uint32_t n1, uint32_t n2);
uint32_t pad(uint32_t n, uint32_t div);
#endif // !MATH_H

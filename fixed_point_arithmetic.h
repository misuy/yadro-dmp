#ifndef _FIXED_POINT_ARITHMETIC
#define _FIXED_POINT_ARITHMETIC

#include <asm-generic/int-ll64.h>

typedef struct uf64 {
    u32 int_part;
    u32 frac_part;
} uf64;

#define UF64_NEW(int_part, frac_part) (uf64) { int_part, frac_part }

#define UF64_FROM_U32(op) UF64_NEW(op, 0)

#define UF64_FROM_COMPOSED_U64(op) UF64_NEW(op >> 32, op)

#define UF64_TO_COMPOSED_U64(op) (u64) (((u64) op.int_part << 32) | op.frac_part)

#define UF64_ADD(op1, op2) UF64_FROM_COMPOSED_U64((UF64_TO_COMPOSED_U64(op1) + UF64_TO_COMPOSED_U64(op2)))

#define UF64_SUB(op1, op2) UF64_FROM_COMPOSED_U64((UF64_TO_COMPOSED_U64(op1) - UF64_TO_COMPOSED_U64(op2)))

#define UF64_MUL(op1, op2) UF64_FROM_COMPOSED_U64((UF64_TO_COMPOSED_U64(op1) * UF64_TO_COMPOSED_U64(op2)))

#define UF64_DIV(op1, op2) UF64_FROM_COMPOSED_U64((UF64_TO_COMPOSED_U64(op1) / UF64_TO_COMPOSED_U64(op2)))

#endif
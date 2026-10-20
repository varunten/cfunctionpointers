/* 02_array_dispatch.c
 *
 * The calculator as an enum-indexed dispatch table. No switch: pick by index,
 * call the function pointer. Uses designated initializers and validates the
 * index before calling (out-of-range indexing would be undefined behavior).
 *
 * Build: gcc 02_array_dispatch.c -o carray && ./carray
 */
#include <stdio.h>

typedef int (*Op)(int, int);

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b)   { return b ? a / b : 0; }

typedef enum { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_COUNT } OpCode;

/* Designated initializers: order-independent, self-documenting, unset = NULL. */
static Op table[OP_COUNT] = {
    [OP_ADD] = add,
    [OP_SUB] = subtract,
    [OP_MUL] = multiply,
    [OP_DIV] = divide,
};

static const char *symbol[OP_COUNT] = {
    [OP_ADD] = "+", [OP_SUB] = "-", [OP_MUL] = "*", [OP_DIV] = "/"
};

int dispatch(OpCode op, int a, int b) {
    if (op < 0 || op >= OP_COUNT || table[op] == NULL) {
        printf("   (invalid op)\n");
        return 0;
    }
    return table[op](a, b);     /* lookup + indirect call replaces the switch */
}

int main(void) {
    for (OpCode op = OP_ADD; op < OP_COUNT; op++)
        printf("12 %s 4 = %d\n", symbol[op], dispatch(op, 12, 4));

    printf("\nTo add an operation: add an enum value, a function, and one table\n");
    printf("row. No central switch to edit.\n");
    return 0;
}

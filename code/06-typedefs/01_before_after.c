/* 01_before_after.c
 *
 * The same idea written two ways: with the raw function-pointer type, and with
 * a typedef. Both compile and run identically; the typedef version is just far
 * easier to read and to change.
 *
 * Build: gcc 01_before_after.c -o ba && ./ba
 */
#include <stdio.h>

int add(int a, int b)      { return a + b; }
int multiply(int a, int b) { return a * b; }

/* BEFORE: raw type spelled out in the parameter list. */
int apply_raw(int (*op)(int, int), int a, int b) {
    return op(a, b);
}

/* AFTER: name the type once... */
typedef int (*BinaryOp)(int, int);

/* ...then use the clean name. */
int apply_typedef(BinaryOp op, int a, int b) {
    return op(a, b);
}

int main(void) {
    printf("raw    : apply_raw(add, 2, 3)        = %d\n", apply_raw(add, 2, 3));
    printf("typedef: apply_typedef(multiply,2,3) = %d\n",
           apply_typedef(multiply, 2, 3));

    printf("\nSame behavior. The typedef version reads better and, if the\n");
    printf("operation signature changed, you'd edit ONE line (the typedef).\n");
    return 0;
}

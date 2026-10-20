/* 02_parens_matter.c
 *
 * The #1 function-pointer gotcha: the parentheses around (*fp).
 *
 *     int (*fp)(int, int);   // POINTER to function -> a variable you assign to
 *     int  *gp(int, int);    // FUNCTION returning int* -> a function declaration
 *
 * The first is what we want. The second declares a function named gp that
 * returns an int*. Same tokens minus parentheses, completely different meaning.
 *
 * Build: gcc 02_parens_matter.c -o parens && ./parens
 */
#include <stdio.h>

int add(int a, int b) { return a + b; }

/* This is the FUNCTION form: gp is a function returning int*.
 * (We define it just to prove it's a function, not a pointer variable.) */
static int storage = 99;
int *gp(int a, int b) {
    storage = a + b;
    return &storage;     /* returns a pointer to int */
}

int main(void) {
    /* The POINTER form: fp is a variable holding a function's address. */
    int (*fp)(int, int) = add;
    printf("fp is a pointer variable. fp(2,3) = %d\n", fp(2, 3));

    /* The FUNCTION form: gp is a function we call directly; it returns int*. */
    int *result = gp(2, 3);
    printf("gp is a function returning int*. *gp(2,3) = %d\n", *result);

    printf("\nWithout the parens, '*' bound to the RETURN TYPE, making a\n");
    printf("function that returns a pointer -- not a pointer to a function.\n");
    return 0;
}

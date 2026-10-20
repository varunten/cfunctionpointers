/* 02_typedef_everywhere.c
 *
 * One typedef, used in all four positions where the raw type would be painful:
 *   - as a variable
 *   - as a function parameter
 *   - as a function RETURN type (this is where raw syntax is worst)
 *   - as a struct field
 *
 * Build: gcc 02_typedef_everywhere.c -o tev && ./tev
 */
#include <stdio.h>

typedef int (*BinaryOp)(int, int);

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }

/* RETURN type: returning a function pointer is trivial to read with a typedef.
 * (Raw, this would be: int (*pick(int))(int, int) -- ugh.) */
BinaryOp pick(int want_add) {
    return want_add ? add : subtract;
}

/* PARAMETER and a struct FIELD: */
struct calculator {
    BinaryOp op;        /* field */
    const char *name;
};

int run(BinaryOp op, int a, int b) {   /* parameter */
    return op(a, b);
}

int main(void) {
    BinaryOp v = add;                  /* variable */
    printf("variable : v(4,5)            = %d\n", v(4, 5));
    printf("parameter: run(subtract,9,4) = %d\n", run(subtract, 9, 4));
    printf("return   : pick(1)(7,8)      = %d\n", pick(1)(7, 8)); /* pick returns add */

    struct calculator c = { pick(0), "subtractor" };  /* field */
    printf("field    : %s c.op(10,3)     = %d\n", c.name, c.op(10, 3));

    printf("\nSame name 'BinaryOp' in every position. That uniformity is\n");
    printf("exactly what makes typedef'd function pointers pleasant.\n");
    return 0;
}

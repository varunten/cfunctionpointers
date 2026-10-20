/* 04_runtime_modify.c
 *
 * A dispatch table is DATA, so you can change it while the program runs --
 * something a switch can never do. Here we swap the "multiply" slot for a
 * logging version at runtime. This is the seed of plugins and hot-swapping
 * (section 16).
 *
 * Build: gcc 04_runtime_modify.c -o cmod && ./cmod
 */
#include <stdio.h>

typedef int (*Op)(int, int);

int multiply(int a, int b)       { return a * b; }
int multiply_loud(int a, int b)  {
    int r = a * b;
    printf("   [log] multiply(%d, %d) = %d\n", a, b, r);
    return r;
}

typedef enum { OP_MUL, OP_COUNT } OpCode;

static Op table[OP_COUNT] = { [OP_MUL] = multiply };

int main(void) {
    printf("Before swap:\n");
    printf("   result = %d\n", table[OP_MUL](6, 7));

    /* Replace the handler at runtime -- impossible with a switch. */
    printf("\nInstalling a logging version of multiply...\n");
    table[OP_MUL] = multiply_loud;

    printf("After swap:\n");
    printf("   result = %d\n", table[OP_MUL](6, 7));

    printf("\nThe call site never changed; the table entry did. Tables let you\n");
    printf("reconfigure behavior at runtime -- the basis of plugin systems.\n");
    return 0;
}

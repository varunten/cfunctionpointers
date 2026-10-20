/* 03_array_of_fps.c
 *
 * An array whose elements are function pointers. Reading:
 *
 *     int (*ops[3])(int, int);
 *     "ops is an array of 3 pointers to functions taking (int,int) returning int"
 *
 * Indexing the array selects a function; adding (a,b) calls it. This is the
 * seed of dispatch tables (section 11).
 *
 * Build: gcc 03_array_of_fps.c -o arr && ./arr
 */
#include <stdio.h>

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }

int main(void) {
    /* The array, initialized with three function addresses. */
    int (*ops[3])(int, int) = { add, subtract, multiply };
    const char *names[3]    = { "add", "subtract", "multiply" };

    int a = 12, b = 4;
    for (int i = 0; i < 3; i++) {
        /* ops[i] is a function pointer; ops[i](a,b) calls it. */
        printf("ops[%d] = %-9s  ops[%d](%d,%d) = %d\n",
               i, names[i], i, a, b, ops[i](a, b));
    }

    printf("\nIndexing picked the function; the (a,b) called it. Replace the\n");
    printf("index with an opcode or a menu choice and you have a dispatcher.\n");
    return 0;
}

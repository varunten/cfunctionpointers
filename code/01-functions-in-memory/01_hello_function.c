/* 01_hello_function.c
 *
 * The absolute starting point: a function is a chunk of machine instructions
 * that the CPU executes. When main() "calls" add(), the CPU jumps to add's
 * instructions, runs them, and comes back.
 *
 * There is nothing about pointers here yet. We just want you to feel that
 * calling a function = "go run the instructions that live somewhere else."
 *
 * Build: gcc 01_hello_function.c -o hello && ./hello
 */
#include <stdio.h>

/* These instructions live in the TEXT (code) segment of the program. */
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

int main(void) {
    /* Each call below makes the CPU jump to that function's code,
     * execute it, and return here with a value. */
    int s = add(2, 3);        /* CPU jumps to add's code,      returns 5  */
    int p = multiply(4, 5);   /* CPU jumps to multiply's code, returns 20 */

    printf("add(2, 3)      = %d\n", s);
    printf("multiply(4, 5) = %d\n", p);

    printf("\nKey idea: 'calling a function' means the CPU went and executed\n");
    printf("instructions that live at some address. Next program: see those\n");
    printf("addresses with your own eyes.\n");
    return 0;
}

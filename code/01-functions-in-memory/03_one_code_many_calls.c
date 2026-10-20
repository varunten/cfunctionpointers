/* 03_one_code_many_calls.c
 *
 * Two ideas people confuse:
 *   1. THE CODE of a function  -> one fixed copy in the TEXT segment.
 *   2. A CALL of that function  -> a stack frame that comes and goes.
 *
 * Recursion makes this vivid: countdown() has ONE code address, but each
 * active call has its OWN local variable at its OWN stack address.
 *
 * Build: gcc 03_one_code_many_calls.c -o frames && ./frames
 */
#include <stdio.h>

void countdown(int n) {
    int marker = n;  /* a fresh 'marker' on the stack for EACH call */

    printf("countdown(%d): code is always at %p, but &marker = %p\n",
           n, (void *)countdown, (void *)&marker);

    if (n > 0)
        countdown(n - 1);  /* a new frame is pushed; same code, new address for marker */
}

int main(void) {
    printf("Watch: the CODE address stays the same on every line,\n");
    printf("but &marker (a stack address) is different each call.\n\n");

    countdown(3);

    printf("\nConclusion: a function pointer points at the CODE (the address\n");
    printf("that never changes), never at a stack frame.\n");
    return 0;
}

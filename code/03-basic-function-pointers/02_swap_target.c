/* 02_swap_target.c
 *
 * One function-pointer variable, pointed at different functions in turn.
 * The whole superpower of function pointers in miniature: changing the stored
 * address changes which function a later call would run.
 *
 * (We print the addresses to make the "stored value changed" idea concrete.
 *  Actually CALLING through fp is section 04 -- previewed on the last lines.)
 *
 * Build: gcc 02_swap_target.c -o swap && ./swap
 */
#include <stdio.h>

int add(int a, int b)      { return a + b; }
int multiply(int a, int b) { return a * b; }

int main(void) {
    int (*fp)(int, int);

    fp = add;
    printf("fp = add;        fp now holds %p (add)\n", (void *)fp);

    fp = multiply;
    printf("fp = multiply;   fp now holds %p (multiply)\n", (void *)fp);

    printf("\nSame variable fp, but the stored address changed -- so a future\n");
    printf("call through fp would run a different function.\n\n");

    /* Preview of section 04: calling through the pointer. */
    fp = add;
    printf("Preview call: fp=add,      fp(2,3) = %d\n", fp(2, 3));
    fp = multiply;
    printf("Preview call: fp=multiply, fp(2,3) = %d\n", fp(2, 3));

    return 0;
}

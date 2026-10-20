/* 01_two_call_styles.c
 *
 * Two ways to call through a function pointer -- they are EXACTLY equivalent:
 *
 *     fp(2, 3)       modern style (call the pointer like a function name)
 *     (*fp)(2, 3)    classic style (dereference, then call)
 *
 * Build: gcc 01_two_call_styles.c -o styles && ./styles
 */
#include <stdio.h>

int add(int a, int b) { return a + b; }

int main(void) {
    int (*fp)(int, int) = add;

    int r1 = fp(2, 3);        /* modern  */
    int r2 = (*fp)(2, 3);     /* classic */

    printf("fp(2, 3)    = %d\n", r1);
    printf("(*fp)(2, 3) = %d\n", r2);
    printf("\nSame result. Both spellings perform the SAME indirect call.\n");
    return 0;
}

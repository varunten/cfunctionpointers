/* 03_two_styles.c
 *
 * Two typedef conventions for function pointers:
 *
 *   Style A: typedef int (*BinaryOp)(int,int);   // the alias IS a pointer
 *            BinaryOp op = add;                   // no * needed
 *
 *   Style B: typedef int BinaryOpFn(int,int);    // the alias is a FUNCTION type
 *            BinaryOpFn *op = add;                // you add the * at use
 *
 * Both end up calling the function the same way.
 *
 * Build: gcc 03_two_styles.c -o styles2 && ./styles2
 */
#include <stdio.h>

int add(int a, int b) { return a + b; }

/* Style A: pointer baked into the typedef. */
typedef int (*BinaryOp)(int, int);

/* Style B: names the function type; pointer added at the use site. */
typedef int BinaryOpFn(int, int);

int main(void) {
    BinaryOp    a = add;     /* already a pointer */
    BinaryOpFn *b = add;     /* add the * yourself */

    printf("Style A: a(2,3) = %d\n", a(2, 3));
    printf("Style B: b(2,3) = %d\n", b(2, 3));

    printf("\nStyle A hides the * (cleanest to use). Style B keeps the *\n");
    printf("visible at the declaration (some teams prefer that). Pick one.\n");
    return 0;
}

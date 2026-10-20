/* 03_direct_vs_indirect.c
 *
 * A direct call and an indirect call, side by side. They produce the same
 * answer, but the CPU reaches the function differently:
 *
 *   DIRECT  : add(2,3)   -> the address of add is a CONSTANT baked into the
 *                           'call' instruction. Compiler knew the target.
 *
 *   INDIRECT: fp(2,3)    -> the CPU first LOADS the address out of fp, then
 *                           calls that. Target decided at runtime.
 *
 * To SEE this, compile to assembly and read the 'call' lines:
 *     gcc -O1 -S 03_direct_vs_indirect.c -o di.s
 * Look for a direct 'call add' vs an indirect 'call *%rax' (or 'call rax').
 *
 * Build & run: gcc 03_direct_vs_indirect.c -o di && ./di
 */
#include <stdio.h>

int add(int a, int b) { return a + b; }

int main(void) {
    /* DIRECT: compiler bakes add's address into the instruction. */
    int direct = add(2, 3);

    /* INDIRECT: compiler emits "load address from fp, then call it". */
    int (*fp)(int, int) = add;
    int indirect = fp(2, 3);

    printf("direct   add(2,3) = %d\n", direct);
    printf("indirect fp(2,3)  = %d\n", indirect);
    printf("\nSame result. The difference is only WHERE the call's target\n");
    printf("address came from: a constant (direct) vs a variable (indirect).\n");
    printf("Compile with -S to see it in the assembly.\n");
    return 0;
}

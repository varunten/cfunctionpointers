/* 02_name_equals_amp.c
 *
 * Proof that the function name, &name, and even *name all denote the SAME
 * address. The value is identical; only the C type differs slightly.
 *
 *   add    : the function (decays to its address in expressions)
 *   &add   : explicitly "address of add"  -- same address
 *   *add   : "dereference the function"... which decays right back to the
 *            address. You can pile on stars and it never changes. Fun proof
 *            that there's really just one address here.
 *
 * Build: gcc 02_name_equals_amp.c -o eq && ./eq
 */
#include <stdio.h>

int add(int a, int b) { return a + b; }

int main(void) {
    printf("add      = %p\n", (void *)add);
    printf("&add     = %p\n", (void *)&add);
    printf("*add     = %p\n", (void *)*add);     /* dereference decays back */
    printf("**add    = %p\n", (void *)**add);    /* still the same! */

    printf("\nAll four lines print the SAME address. 'add' and '&add' are\n");
    printf("interchangeable when assigning to a function pointer. The chain\n");
    printf("of *'s proves there is exactly one address being named here.\n");
    return 0;
}

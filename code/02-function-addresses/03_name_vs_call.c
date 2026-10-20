/* 03_name_vs_call.c
 *
 * The single most important distinction in this whole series:
 *
 *     greet        -> a VALUE: the address of greet's code (no action)
 *     greet()      -> an ACTION: jump to that address and run it
 *
 * () is the "call operator". No parentheses, no call.
 *
 * Build: gcc 03_name_vs_call.c -o nc && ./nc
 */
#include <stdio.h>

void greet(void) {
    printf("   >>> greet() actually ran! <<<\n");
}

int main(void) {
    printf("Line A: naming the function (no parentheses):\n");
    printf("   greet as a value (its address) = %p\n", (void *)greet);
    printf("   ...notice greet did NOT run. We only mentioned its address.\n\n");

    printf("Line B: calling the function (with parentheses):\n");
    greet();  /* NOW it runs */
    printf("   ...that time greet ran, because we used () to call it.\n\n");

    printf("Remember: function pointers store the VALUE (the address).\n");
    printf("Calling through a pointer just adds the () later.\n");
    return 0;
}

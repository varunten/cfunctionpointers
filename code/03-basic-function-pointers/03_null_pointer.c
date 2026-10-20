/* 03_null_pointer.c
 *
 * A function pointer can be NULL -- "points at no function yet" -- exactly like
 * a NULL data pointer. Calling through NULL is undefined behavior (a crash), so
 * the universal pattern for optional callbacks is: guard with "if (fp)".
 *
 * Build: gcc 03_null_pointer.c -o nullp && ./nullp
 */
#include <stdio.h>

void on_event(void) { printf("   handler ran!\n"); }

int main(void) {
    void (*handler)(void) = NULL;   /* no handler installed yet */

    printf("Attempt 1 (handler is NULL):\n");
    if (handler != NULL)
        handler();
    else
        printf("   no handler installed -- skipping (this is the safe path)\n");

    printf("\nInstalling a handler...\n");
    handler = on_event;

    printf("Attempt 2 (handler installed):\n");
    if (handler != NULL)
        handler();

    printf("\nThe 'if (fp)' guard is exactly how real callback APIs decide\n");
    printf("whether the user supplied behavior. NULL means 'no behavior'.\n");
    return 0;
}

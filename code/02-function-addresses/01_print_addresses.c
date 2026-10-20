/* 01_print_addresses.c
 *
 * Every function has an address (where its code starts in the TEXT segment).
 * Here we just print several of them. Different functions -> different
 * addresses. Run it twice: numbers may change between runs (ASLR), but within
 * one run they're fixed, and the functions are always distinct.
 *
 * Build: gcc 01_print_addresses.c -o addrs && ./addrs
 *
 * Note: casting a function pointer to (void*) for %p is fine on normal PCs.
 * It may warn on pedantic settings; that's harmless here (see theory sec. 5).
 */
#include <stdio.h>

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }

int main(void) {
    printf("add      lives at: %p\n", (void *)add);
    printf("subtract lives at: %p\n", (void *)subtract);
    printf("multiply lives at: %p\n", (void *)multiply);
    printf("main     lives at: %p\n", (void *)main);

    printf("\nEach function has its OWN address. That's what lets us later\n");
    printf("store ONE of these addresses in a variable and choose which\n");
    printf("function to run -- that variable is a function pointer.\n");
    return 0;
}

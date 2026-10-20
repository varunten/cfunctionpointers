/* 01_declare_and_store.c
 *
 * Declare a function pointer and store a function's address in it.
 *
 *   int (*fp)(int, int);   // fp is a variable that holds the address of a
 *                          // function taking (int,int) and returning int.
 *   fp = add;              // store add's address into fp
 *
 * Then prove fp holds exactly add's address by printing both.
 *
 * Build: gcc 01_declare_and_store.c -o store && ./store
 */
#include <stdio.h>

int add(int a, int b) { return a + b; }

int main(void) {
    int (*fp)(int, int);   /* the function-pointer VARIABLE (see sec 05 for syntax) */

    fp = add;              /* store the address of add's code into fp */

    printf("address of add       : %p\n", (void *)add);
    printf("value stored in fp   : %p\n", (void *)fp);
    printf("size of fp (bytes)   : %zu\n", sizeof fp);

    if ((void *)fp == (void *)add)
        printf("\nfp holds EXACTLY add's address. fp is a box containing the\n"
               "place where add's code begins.\n");

    return 0;
}

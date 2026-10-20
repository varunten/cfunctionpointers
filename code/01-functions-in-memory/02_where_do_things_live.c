/* 02_where_do_things_live.c
 *
 * Goal: SEE the memory segments from the theory diagram by printing the
 * address of one thing from each region. The exact numbers vary by OS and
 * change between runs (ASLR), but the RELATIVE ORDERING usually holds:
 *
 *     code (TEXT)  <  globals (DATA/BSS)  <  heap  <  stack
 *     (low addresses)  ............................  (high addresses)
 *
 * Build: gcc 02_where_do_things_live.c -o layout && ./layout
 */
#include <stdio.h>
#include <stdlib.h>

int global_initialized = 100;  /* DATA segment */
int global_zeroed;             /* BSS  segment */

/* A function: its code lives in the TEXT segment. */
void some_function(void) { /* does nothing on purpose */ }

int main(void) {
    int local_var = 7;                 /* STACK */
    int *heap_var  = malloc(sizeof(int)); /* the int is on the HEAP */

    printf("=== Where does each kind of thing live? ===\n\n");

    /* %p prints a pointer (an address). We cast everything to void* /
       function pointer just so printf is happy. */
    printf("TEXT  (code)   some_function : %p\n", (void *)some_function);
    printf("TEXT  (code)   main          : %p\n", (void *)main);
    printf("DATA  (init'd) &global_init  : %p\n", (void *)&global_initialized);
    printf("BSS   (zeroed) &global_zeroed: %p\n", (void *)&global_zeroed);
    printf("HEAP  (malloc)  heap_var     : %p\n", (void *)heap_var);
    printf("STACK (local)  &local_var    : %p\n", (void *)&local_var);

    printf("\nLook at the ordering. Code is typically at the LOW end,\n");
    printf("the stack at the HIGH end -- exactly the theory diagram.\n");
    printf("\nNote the two code addresses above are real function addresses.\n");
    printf("In section 03 we'll STORE one of them in a pointer variable.\n");

    free(heap_var);
    return 0;
}

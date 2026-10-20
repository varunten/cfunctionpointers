/* 04_decode_declarations.c
 *
 * A reading drill. Each declaration below is one you might meet in real headers.
 * The comments show how to decode it with the Right-Left rule and the
 * "mental typedef" trick. Every pointer is actually used so the file runs.
 *
 * Build: gcc 04_decode_declarations.c -o rdecode && ./rdecode
 */
#include <stdio.h>
#include <stdlib.h>

int  add(int a, int b)        { return a + b; }
void greet(void)              { printf("   hi\n"); }
int  is_even(int v)           { return v % 2 == 0; }

int main(void) {
    /* (1)  int (*f)(int, int);
       Right-Left: f is a pointer to a function taking (int,int) returning int. */
    int (*f)(int, int) = add;
    printf("(1) f(3,4)        = %d\n", f(3, 4));

    /* (2)  void (*g)(void);
       g is a pointer to a function taking no args, returning nothing. */
    void (*g)(void) = greet;
    printf("(2) g() -> ");  g();

    /* (3)  int (*pred)(int);
       pred is a pointer to a function taking int returning int (a predicate). */
    int (*pred)(int) = is_even;
    printf("(3) pred(10)      = %d\n", pred(10));

    /* (4)  int (*ops[2])(int, int);
       ops is an ARRAY OF 2 pointers to functions taking (int,int) returning int.
       Mental typedef: typedef int (*Op)(int,int); Op ops[2]; */
    int (*ops[2])(int, int) = { add, add };
    printf("(4) ops[1](5,6)   = %d\n", ops[1](5, 6));

    /* (5)  The scary one -- like real qsort's parameter:
            void qsort(void*, size_t, size_t, int (*)(const void*, const void*));
       Mental typedef: typedef int (*Cmp)(const void*, const void*);
       then it's just: void qsort(void*, size_t, size_t, Cmp);  -- a comparator. */
    /* (already practiced in 01_like_qsort.c) */
    printf("(5) qsort's 4th arg is a Comparator: int (*)(const void*,const void*)\n");

    /* (6)  The infamous signal:  void (*signal(int, void (*)(int)))(int);
       Mental typedef: typedef void (*Handler)(int);
       then: Handler signal(int, Handler);  -- takes a handler, returns one. */
    printf("(6) signal becomes readable as: Handler signal(int, Handler)\n");

    printf("\nTrick: find the repeated function-pointer shape, name it with a\n");
    printf("mental typedef, and the declaration reads like English.\n");
    return 0;
}

/* 01_for_each.c
 *
 * The essence of a callback: for_each() owns the LOOP (the mechanism); you
 * supply the function it runs on each element (the policy). Same loop, totally
 * different behavior, just by passing a different function.
 *
 * Build: gcc 01_for_each.c -o foreach && ./foreach
 */
#include <stdio.h>

typedef void (*Visitor)(int value);

/* The library function. It does not know or care WHAT visit does. */
void for_each(int *arr, int n, Visitor visit) {
    for (int i = 0; i < n; i++)
        visit(arr[i]);          /* the callback: for_each calls YOUR function */
}

/* Two different policies the caller can inject: */
void print_it(int v)   { printf("%d ", v); }
void print_sq(int v)   { printf("%d ", v * v); }

int main(void) {
    int arr[] = { 1, 2, 3, 4, 5 };
    int n = (int)(sizeof arr / sizeof arr[0]);

    printf("print each:    ");
    for_each(arr, n, print_it);
    printf("\n");

    printf("print squares: ");
    for_each(arr, n, print_sq);
    printf("\n");

    printf("\nThe loop in for_each was written ONCE. The behavior changed only\n");
    printf("because we passed a different callback. That's inversion of control.\n");
    return 0;
}

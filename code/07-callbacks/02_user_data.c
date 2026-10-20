/* 02_user_data.c
 *
 * Callbacks usually need CONTEXT. The standard C trick: the library passes a
 * 'void *user_data' straight through to your callback, untouched. You stash
 * whatever state you need behind it and cast it back inside the callback.
 *
 * Here we SUM an array by accumulating through user_data, and also COUNT how
 * many are even -- two different states carried via void *.
 *
 * Build: gcc 02_user_data.c -o udata && ./udata
 */
#include <stdio.h>

typedef void (*Visitor)(int value, void *user_data);

/* Library: passes user_data back to you on every call. It never looks inside. */
void for_each(int *arr, int n, Visitor visit, void *user_data) {
    for (int i = 0; i < n; i++)
        visit(arr[i], user_data);
}

/* Policy 1: accumulate a sum. user_data is really an int*. */
void accumulate(int v, void *user_data) {
    int *total = user_data;     /* cast the opaque pointer back to its real type */
    *total += v;
}

/* Policy 2: count evens. user_data is again an int*. */
void count_even(int v, void *user_data) {
    int *count = user_data;
    if (v % 2 == 0)
        (*count)++;
}

int main(void) {
    int arr[] = { 3, 8, 5, 2, 10, 7 };
    int n = (int)(sizeof arr / sizeof arr[0]);

    int total = 0;
    for_each(arr, n, accumulate, &total);   /* hand the library a ptr to our total */
    printf("sum of elements = %d\n", total);

    int evens = 0;
    for_each(arr, n, count_even, &evens);
    printf("count of evens  = %d\n", evens);

    printf("\nfor_each carried our own variables back to us via 'void *',\n");
    printf("so the callback could update state we own. That's the standard\n");
    printf("C way to give callbacks context.\n");
    return 0;
}

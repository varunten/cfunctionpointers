/* 04_bsearch.c
 *
 * bsearch uses the SAME comparator shape as qsort. So one ordering policy serves
 * two algorithms: sort the array with qsort, then search it with bsearch, both
 * driven by the same callback. The array MUST be sorted first.
 *
 * Build: gcc 04_bsearch.c -o bsrch && ./bsrch
 */
#include <stdio.h>
#include <stdlib.h>

int asc(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    if (x < y) return -1;
    if (x > y) return  1;
    return 0;
}

int main(void) {
    int arr[] = { 50, 10, 40, 20, 30 };
    int n = (int)(sizeof arr / sizeof arr[0]);

    qsort(arr, n, sizeof(int), asc);   /* bsearch needs a sorted array */

    printf("sorted: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    int key = 30;
    int *found = bsearch(&key, arr, n, sizeof(int), asc);  /* same comparator! */

    if (found)
        printf("found %d at index %ld\n", key, (long)(found - arr));
    else
        printf("%d not found\n", key);

    printf("\nOne comparator, two algorithms (qsort + bsearch). Clean callback\n");
    printf("contracts let different mechanisms share the same injected policy.\n");
    return 0;
}

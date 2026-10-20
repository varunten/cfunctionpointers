/* 01_qsort_int.c
 *
 * Sort an int array with qsort. The ONLY thing that changes between ascending
 * and descending is which comparator callback we pass.
 *
 * Build: gcc 01_qsort_int.c -o qint && ./qint
 */
#include <stdio.h>
#include <stdlib.h>

/* Ascending: return negative if *a should come before *b.
 * Using explicit comparisons (safe; the subtraction trick can overflow). */
int asc(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    if (x < y) return -1;
    if (x > y) return  1;
    return 0;
}

/* Descending: just flip the comparisons. */
int desc(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    if (x > y) return -1;
    if (x < y) return  1;
    return 0;
}

void print_arr(const char *label, int *arr, int n) {
    printf("%s", label);
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");
}

int main(void) {
    int arr[] = { 5, 2, 9, 1, 7, 3 };
    int n = (int)(sizeof arr / sizeof arr[0]);

    print_arr("original:   ", arr, n);

    qsort(arr, n, sizeof(int), asc);
    print_arr("ascending:  ", arr, n);

    qsort(arr, n, sizeof(int), desc);
    print_arr("descending: ", arr, n);

    printf("\nSame qsort, same array -- only the comparator changed.\n");
    return 0;
}

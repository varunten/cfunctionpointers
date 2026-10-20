/* 01_like_qsort.c
 *
 * Recognizing the comparator pattern. We implement a tiny generic sort with the
 * EXACT signature shape of qsort, so you learn to read it. When you see
 * "(const void*, const void*) -> int", think: comparator callback.
 *
 * Build: gcc 01_like_qsort.c -o rqsort && ./rqsort
 */
#include <stdio.h>
#include <string.h>

/* Same shape as the real qsort. */
typedef int (*Comparator)(const void *, const void *);

void my_sort(void *base, size_t n, size_t size, Comparator cmp) {
    char *arr = base;
    /* simple bubble sort; the point is HOW it uses the callback + size */
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j + 1 < n - i; j++) {
            char *a = arr + j * size;
            char *b = arr + (j + 1) * size;
            if (cmp(a, b) > 0) {                /* ask the callback: a after b? */
                /* swap 'size' bytes -- we don't know the element type! */
                for (size_t k = 0; k < size; k++) {
                    char t = a[k]; a[k] = b[k]; b[k] = t;
                }
            }
        }
    }
}

int cmp_int_asc(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);     /* -1, 0, or +1 without overflow */
}

int main(void) {
    int arr[] = { 5, 2, 9, 1, 7 };
    int n = (int)(sizeof arr / sizeof arr[0]);

    my_sort(arr, n, sizeof(int), cmp_int_asc);

    printf("sorted: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n\nThis is qsort's exact shape: a generic algorithm + a comparator\n");
    printf("callback over const void*. Recognize it and qsort holds no secrets.\n");
    return 0;
}

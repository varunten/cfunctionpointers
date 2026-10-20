/* 03_trace_compare.c
 *
 * Watch qsort call back into YOUR comparator. Each printed line is an indirect
 * call from inside the library into your function -- the section-04 mechanism,
 * happening for real. The exact pairs and order depend on your libc's sort.
 *
 * Build: gcc 03_trace_compare.c -o qtrace && ./qtrace
 */
#include <stdio.h>
#include <stdlib.h>

static int call_count = 0;

int asc_traced(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    call_count++;
    printf("   compare call #%d: is %d before %d?  -> %s\n",
           call_count, x, y, (x < y) ? "yes (-)" : (x > y) ? "no (+)" : "equal (0)");
    if (x < y) return -1;
    if (x > y) return  1;
    return 0;
}

int main(void) {
    int arr[] = { 3, 1, 2 };
    int n = (int)(sizeof arr / sizeof arr[0]);

    printf("Sorting {3, 1, 2}. qsort will call our comparator repeatedly:\n\n");
    qsort(arr, n, sizeof(int), asc_traced);

    printf("\nResult: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n\nqsort asked %d ordering questions; we answered each one.\n",
           call_count);
    printf("qsort did the sorting; we only judged pairs. Mechanism vs policy.\n");
    return 0;
}

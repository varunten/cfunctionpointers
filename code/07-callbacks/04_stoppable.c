/* 04_stoppable.c
 *
 * A powerful refinement: let the callback's RETURN VALUE control the host loop.
 * Convention here: return nonzero to STOP iterating. This is how "find first
 * match" / "iterate until done" APIs are built (e.g. directory walkers).
 *
 * Build: gcc 04_stoppable.c -o stoppable && ./stoppable
 */
#include <stdio.h>

/* Callback returns 0 to continue, nonzero to stop. */
typedef int (*Visitor)(int value, void *user_data);

/* Library: stops early if the callback asks it to. */
void for_each(int *arr, int n, Visitor visit, void *user_data) {
    for (int i = 0; i < n; i++) {
        if (visit(arr[i], user_data) != 0)   /* callback decides when to quit */
            break;
    }
}

/* Find the first value greater than a threshold. Stops the loop when found. */
struct search { int threshold; int found; };

int find_above(int v, void *user_data) {
    struct search *s = user_data;
    if (v > s->threshold) {
        s->found = v;
        return 1;            /* nonzero: stop the loop */
    }
    return 0;                /* keep going */
}

int main(void) {
    int arr[] = { 2, 4, 6, 9, 11, 3 };
    int n = (int)(sizeof arr / sizeof arr[0]);

    struct search s = { .threshold = 5, .found = -1 };
    for_each(arr, n, find_above, &s);

    if (s.found != -1)
        printf("first value above %d is %d\n", s.threshold, s.found);
    else
        printf("no value above %d\n", s.threshold);

    printf("\nThe host owned the loop, but the callback decided when to stop.\n");
    printf("Returning a value from a callback lets it steer the host.\n");
    return 0;
}

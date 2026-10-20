/* 03_optional_callback.c
 *
 * Callbacks are often optional. The library calls them only if you supplied one,
 * using the NULL guard. Passing NULL cleanly means "no behavior here."
 *
 * Build: gcc 03_optional_callback.c -o optcb && ./optcb
 */
#include <stdio.h>

typedef void (*ProgressFn)(int percent);

/* Simulated work that reports progress IF a callback was given. */
void do_work(int steps, ProgressFn on_progress) {
    for (int i = 1; i <= steps; i++) {
        /* ...pretend to do a chunk of work... */
        if (on_progress)                       /* the guard: only call if non-NULL */
            on_progress(i * 100 / steps);
    }
}

void show_progress(int percent) {
    printf("   progress: %d%%\n", percent);
}

int main(void) {
    printf("With a progress callback:\n");
    do_work(4, show_progress);

    printf("\nWithout one (pass NULL -- opt out of progress):\n");
    do_work(4, NULL);
    printf("   (work still happened; we just chose not to be notified)\n");

    printf("\nNULL is how a caller says 'I don't want this behavior'.\n");
    return 0;
}

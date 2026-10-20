/* 04_lifetime_demo.c
 *
 * The #1 callback-API bug: storing a callback with user_data that points at
 * memory which dies before the callback runs (asynchronous case). This file
 * shows the CORRECT "caller-owns, keep it alive" approach, and explains in
 * comments exactly what would go wrong otherwise.
 *
 * Build: gcc 04_lifetime_demo.c -o lifeapi && ./lifeapi
 */
#include <stdio.h>

typedef void (*Callback)(void *user_data);

/* The library stores the callback to call LATER (async-style). */
static Callback g_cb = NULL;
static void    *g_cb_data = NULL;

void library_register(Callback cb, void *user_data) {
    g_cb = cb;
    g_cb_data = user_data;
}

void library_fire_later(void) {     /* called some time after registration */
    if (g_cb)
        g_cb(g_cb_data);
}

/* user callback: reads its context through user_data */
void on_done(void *user_data) {
    int *counter = user_data;
    printf("   callback ran; counter = %d\n", *counter);
}

/*
 * WRONG (do NOT do this):
 *
 *   void register_bad(void) {
 *       int local = 42;
 *       library_register(on_done, &local);   // &local dies when this returns!
 *   }                                        // later, firing reads freed stack -> UB
 *
 * The fix used below: the data outlives the registration (it lives in main,
 * which is still running when the callback fires). For truly async lifetimes
 * you'd use heap memory + a destroy callback, or have the library copy the data.
 */

int main(void) {
    int counter = 42;                       /* lives for the whole program */

    library_register(on_done, &counter);    /* counter will still be alive when fired */

    printf("Registered. Doing other work...\n");
    counter = 100;                          /* mutate through the same memory */

    printf("Now firing the stored callback:\n");
    library_fire_later();                   /* reads counter (==100) safely */

    printf("\nThe callback's data (counter) outlived the registration, so the\n");
    printf("deferred call was safe. That lifetime guarantee is the caller's job.\n");
    return 0;
}

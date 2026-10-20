/* 02_like_pthread.c
 *
 * Recognizing the "run this function, with this data, over there" pattern --
 * the shape of pthread_create:
 *
 *     int pthread_create(..., void *(*start_routine)(void *), void *arg);
 *
 * We don't actually spawn a thread (portability); we just call the routine to
 * show the SHAPE: a void*(*)(void*) callback plus a void *arg user_data.
 *
 * Build: gcc 02_like_pthread.c -o rthread && ./rthread
 */
#include <stdio.h>
#include <stdint.h>

/* Same shape as pthread_create's start_routine. */
typedef void *(*StartRoutine)(void *arg);

/* Stand-in for pthread_create: in reality this runs 'routine' on a new thread,
 * passing 'arg'. Here we just invoke it directly to illustrate the contract. */
int run_task(StartRoutine routine, void *arg) {
    void *result = routine(arg);          /* the "thread" runs your function */
    printf("   task returned %ld\n", (long)(intptr_t)result);
    return 0;
}

/* A worker: receives its data via void *arg (the user_data channel). */
struct work { int a, b; };

void *adder(void *arg) {
    struct work *w = arg;                 /* cast the opaque arg back */
    int sum = w->a + w->b;
    printf("   adder computing %d + %d\n", w->a, w->b);
    return (void *)(intptr_t)sum;         /* return via void* */
}

int main(void) {
    struct work w = { 20, 22 };
    printf("Running a task (pthread_create shape):\n");
    run_task(adder, &w);                  /* function + its data */

    printf("\nThat is pthread_create's contract: void*(*)(void*) + void *arg.\n");
    printf("Real pthreads run it asynchronously -- so 'arg' must outlive the\n");
    printf("call (lifetime, section 15). Recognize the shape, recall the rule.\n");
    return 0;
}

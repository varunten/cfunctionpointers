/* 02_turnstile_fnptr.c
 *
 * The same turnstile, but each STATE is a FUNCTION and the current state is a
 * FUNCTION POINTER. A transition is just reassigning that pointer:
 *
 *     m->current = state_unlocked;
 *
 * Each state's logic lives in its own small function -- no central switch.
 *
 * Build: gcc 02_turnstile_fnptr.c -o tfp && ./tfp
 */
#include <stdio.h>
#include <string.h>

struct machine;
typedef void (*StateFn)(struct machine *m, const char *input);

struct machine {
    StateFn     current;   /* the current state IS this function pointer */
    const char *name;
};

/* forward declarations so states can transition to each other */
void state_locked(struct machine *m, const char *input);
void state_unlocked(struct machine *m, const char *input);

void state_locked(struct machine *m, const char *input) {
    if (strcmp(input, "coin") == 0) {
        printf("   *clunk* unlocking\n");
        m->current = state_unlocked;   /* TRANSITION */
        m->name    = "UNLOCKED";
    } else {
        printf("   *clack* stays locked\n");
    }
}

void state_unlocked(struct machine *m, const char *input) {
    if (strcmp(input, "push") == 0) {
        printf("   *thunk* locking\n");
        m->current = state_locked;     /* TRANSITION */
        m->name    = "LOCKED";
    } else {
        printf("   coin ignored (already unlocked)\n");
    }
}

int main(void) {
    struct machine m = { state_locked, "LOCKED" };
    const char *inputs[] = { "push", "coin", "coin", "push", "push" };
    int n = (int)(sizeof inputs / sizeof inputs[0]);

    for (int i = 0; i < n; i++) {
        printf("[%s] + %-4s ->\n", m.name, inputs[i]);
        m.current(&m, inputs[i]);      /* run the current state */
        printf("   now: %s\n", m.name);
    }

    printf("\nThe machine's 'position' was a single function pointer. Each\n");
    printf("transition just reassigned it. No giant switch in sight.\n");
    return 0;
}

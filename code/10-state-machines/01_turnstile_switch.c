/* 01_turnstile_switch.c
 *
 * Baseline turnstile FSM using an enum + switch. This is the familiar style.
 * Compare it with 02_turnstile_fnptr.c to see the function-pointer alternative.
 *
 * States: LOCKED, UNLOCKED. Inputs: "coin", "push".
 *
 * Build: gcc 01_turnstile_switch.c -o tsw && ./tsw
 */
#include <stdio.h>
#include <string.h>

typedef enum { LOCKED, UNLOCKED } State;

const char *name(State s) { return s == LOCKED ? "LOCKED" : "UNLOCKED"; }

State next(State s, const char *input) {
    switch (s) {
        case LOCKED:
            if (strcmp(input, "coin") == 0) { printf("   *clunk* unlocking\n"); return UNLOCKED; }
            printf("   *clack* stays locked\n");
            return LOCKED;
        case UNLOCKED:
            if (strcmp(input, "push") == 0) { printf("   *thunk* locking\n"); return LOCKED; }
            printf("   coin ignored (already unlocked)\n");
            return UNLOCKED;
    }
    return s;
}

int main(void) {
    State s = LOCKED;
    const char *inputs[] = { "push", "coin", "coin", "push", "push" };
    int n = (int)(sizeof inputs / sizeof inputs[0]);

    for (int i = 0; i < n; i++) {
        printf("[%s] + %-4s ->\n", name(s), inputs[i]);
        s = next(s, inputs[i]);
        printf("   now: %s\n", name(s));
    }
    return 0;
}

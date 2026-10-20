/* 03_traffic_light.c
 *
 * A 3-state cycle: GREEN -> YELLOW -> RED -> GREEN -> ...
 * No external input needed; each "tick" the current state runs, prints itself,
 * and points the machine at the next state. The current state is a function
 * pointer; transitions are reassignments.
 *
 * Build: gcc 03_traffic_light.c -o light && ./light
 */
#include <stdio.h>

struct light;
typedef void (*StateFn)(struct light *L);

struct light { StateFn current; };

void green(struct light *L);
void yellow(struct light *L);
void red(struct light *L);

void green(struct light *L) {
    printf("GREEN  - go\n");
    L->current = yellow;     /* next tick will be yellow */
}
void yellow(struct light *L) {
    printf("YELLOW - slow down\n");
    L->current = red;
}
void red(struct light *L) {
    printf("RED    - stop\n");
    L->current = green;
}

int main(void) {
    struct light L = { green };

    printf("Running 7 ticks of the traffic light:\n");
    for (int tick = 0; tick < 7; tick++)
        L.current(&L);       /* run current state; it sets up the next */

    printf("\nThe cycle advances purely by each state reassigning the\n");
    printf("function pointer to the next state.\n");
    return 0;
}

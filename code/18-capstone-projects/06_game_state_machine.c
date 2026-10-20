/* 06_game_state_machine.c  -- Capstone Project 6: a text game state machine.
 *
 * The game's current state is a function pointer (menu / playing / gameover).
 * Each state interprets input and may transition by reassigning the pointer.
 * Patterns: state machines (10), state-carried data (10,13).
 *
 * The demo feeds a scripted sequence of inputs so it runs non-interactively.
 *
 * Build: gcc 06_game_state_machine.c -o game && ./game
 */
#include <stdio.h>
#include <string.h>

struct game;
typedef void (*StateFn)(struct game *g, const char *input);

struct game {
    StateFn     current;
    const char *state_name;
    int         score;
    int         lives;
    int         over;        /* set when the machine reaches a terminal state */
};

/* forward declarations so states can transition to one another */
static void state_menu(struct game *g, const char *input);
static void state_playing(struct game *g, const char *input);
static void state_gameover(struct game *g, const char *input);

static void goto_state(struct game *g, StateFn fn, const char *name) {
    g->current = fn;
    g->state_name = name;
    printf("   -> now in %s\n", name);
}

static void state_menu(struct game *g, const char *input) {
    if (strcmp(input, "start") == 0) {
        g->score = 0; g->lives = 3;
        goto_state(g, state_playing, "PLAYING");
    } else if (strcmp(input, "quit") == 0) {
        g->over = 1;
        printf("   (left the menu)\n");
    } else {
        printf("   menu: type 'start' or 'quit'\n");
    }
}

static void state_playing(struct game *g, const char *input) {
    if (strcmp(input, "score") == 0) {
        g->score += 10;
        printf("   scored! score=%d lives=%d\n", g->score, g->lives);
    } else if (strcmp(input, "hit") == 0) {
        g->lives--;
        printf("   ouch! score=%d lives=%d\n", g->score, g->lives);
        if (g->lives <= 0)
            goto_state(g, state_gameover, "GAMEOVER");
    } else {
        printf("   playing: type 'score' or 'hit'\n");
    }
}

static void state_gameover(struct game *g, const char *input) {
    if (strcmp(input, "menu") == 0) {
        goto_state(g, state_menu, "MENU");
    } else {
        printf("   game over. final score=%d. type 'menu'\n", g->score);
    }
}

int main(void) {
    struct game g = { state_menu, "MENU", 0, 0, 0 };

    const char *inputs[] = {
        "start", "score", "score", "hit", "hit", "hit", /* lives 3->0 -> gameover */
        "score",          /* ignored in gameover */
        "menu",           /* back to menu */
        "quit",
    };
    int n = (int)(sizeof inputs / sizeof inputs[0]);

    for (int i = 0; i < n && !g.over; i++) {
        printf("[%s] input: %s\n", g.state_name, inputs[i]);
        g.current(&g, inputs[i]);     /* run the current state */
    }

    printf("\nThe game's behavior changed entirely based on which state\n");
    printf("function the pointer held. Transitions were just reassignments.\n");
    return 0;
}

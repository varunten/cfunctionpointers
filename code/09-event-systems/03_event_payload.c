/* 03_event_payload.c
 *
 * Real events carry DATA (what happened) and handlers usually need their own
 * CONTEXT too. So handlers take (const Event*, void *user_data). The dispatcher
 * passes both to every listener.
 *
 * Build: gcc 03_event_payload.c -o payload && ./payload
 */
#include <stdio.h>

#define MAX_LISTENERS 16

typedef struct {
    int x, y;       /* where the click happened */
    int button;     /* which button (0=left, 1=right) */
} ClickEvent;

/* A handler gets the event AND its own user_data context. */
typedef void (*ClickListener)(const ClickEvent *ev, void *user_data);

struct event {
    ClickListener listeners[MAX_LISTENERS];
    void         *user_data[MAX_LISTENERS];   /* paired context per listener */
    int count;
};

void on(struct event *e, ClickListener fn, void *ud) {
    if (e->count < MAX_LISTENERS) {
        e->listeners[e->count] = fn;
        e->user_data[e->count] = ud;
        e->count++;
    }
}

void emit_click(struct event *e, const ClickEvent *ev) {
    for (int i = 0; i < e->count; i++)
        e->listeners[i](ev, e->user_data[i]);   /* pass event + that listener's context */
}

/* One listener prints coordinates. */
void show_coords(const ClickEvent *ev, void *ud) {
    (void)ud;
    printf("   clicked at (%d, %d) with button %d\n", ev->x, ev->y, ev->button);
}

/* Another counts clicks into a counter we own (via user_data). */
void count_clicks(const ClickEvent *ev, void *ud) {
    (void)ev;
    int *counter = ud;
    (*counter)++;
    printf("   total clicks so far: %d\n", *counter);
}

int main(void) {
    struct event on_click = { .count = 0 };
    int clicks = 0;

    on(&on_click, show_coords, NULL);
    on(&on_click, count_clicks, &clicks);   /* hand this listener our counter */

    ClickEvent e1 = { .x = 10, .y = 20, .button = 0 };
    ClickEvent e2 = { .x = 33, .y =  7, .button = 1 };

    printf("First click:\n");  emit_click(&on_click, &e1);
    printf("Second click:\n"); emit_click(&on_click, &e2);

    printf("\nThe event told handlers WHAT happened; user_data told one of them\n");
    printf("WHERE to keep its state. Together they make handlers fully general.\n");
    return 0;
}

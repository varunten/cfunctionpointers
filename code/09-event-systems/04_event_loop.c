/* 04_event_loop.c
 *
 * A miniature EVENT LOOP -- the heart of every GUI, game engine, and network
 * server. We have a queue of events; the loop pulls each one and dispatches it
 * to the handler registered for that event TYPE (a small dispatch table of
 * function pointers, previewing section 11).
 *
 * Build: gcc 04_event_loop.c -o loop && ./loop
 */
#include <stdio.h>

typedef enum { EV_KEY, EV_CLICK, EV_QUIT, EV_TYPE_COUNT } EventType;

typedef struct {
    EventType type;
    int       data;     /* key code, or click id, etc. */
} Event;

typedef void (*Handler)(const Event *ev);

/* One handler per event type. */
void on_key(const Event *ev)   { printf("   key pressed: %d\n", ev->data); }
void on_click(const Event *ev) { printf("   clicked target #%d\n", ev->data); }
void on_quit(const Event *ev)  { (void)ev; printf("   quit requested\n"); }

int main(void) {
    /* Dispatch table: event type -> handler. */
    Handler handlers[EV_TYPE_COUNT] = {
        [EV_KEY]   = on_key,
        [EV_CLICK] = on_click,
        [EV_QUIT]  = on_quit,
    };

    /* A pretend event queue (in a real program these arrive over time). */
    Event queue[] = {
        { EV_KEY,   65 },
        { EV_CLICK,  3 },
        { EV_KEY,   66 },
        { EV_QUIT,   0 },
    };
    int n = (int)(sizeof queue / sizeof queue[0]);

    int running = 1;
    printf("Event loop running...\n");
    for (int i = 0; i < n && running; i++) {
        Event ev = queue[i];                 /* "wait_for_next_event()" */
        printf("dispatching event type %d:\n", ev.type);
        if (handlers[ev.type])
            handlers[ev.type](&ev);          /* turn the event into a call */
        if (ev.type == EV_QUIT)
            running = 0;                      /* stop the loop */
    }
    printf("Event loop stopped.\n");

    printf("\nThis tiny loop -- take an event, call the right function pointer,\n");
    printf("repeat -- is the skeleton of essentially every interactive program.\n");
    return 0;
}

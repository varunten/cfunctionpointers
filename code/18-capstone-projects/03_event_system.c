/* 03_event_system.c  -- Capstone Project 3: a reusable event bus / observer.
 *
 * Register listeners per event type, emit events with a payload, unregister by
 * id. Patterns: events (09), callbacks + user_data (07), register API (15).
 *
 * Build: gcc 03_event_system.c -o eventbus && ./eventbus
 */
#include <stdio.h>
#include <string.h>

typedef enum { EV_LOGIN, EV_LOGOUT, EV_MESSAGE, EV_TYPE_COUNT } EventType;

typedef struct {
    EventType   type;
    const char *who;
    const char *text;     /* for EV_MESSAGE */
} Event;

typedef void (*Listener)(const Event *ev, void *user_data);

#define MAX_SUBS 32
struct sub {
    int        used;
    EventType  type;
    Listener   fn;
    void      *user_data;
};

typedef struct { struct sub subs[MAX_SUBS]; } EventBus;

int bus_on(EventBus *b, EventType t, Listener fn, void *ud) {
    for (int i = 0; i < MAX_SUBS; i++) {
        if (!b->subs[i].used) {
            b->subs[i] = (struct sub){ .used = 1, .type = t, .fn = fn, .user_data = ud };
            return i;                       /* registration id */
        }
    }
    return -1;
}

void bus_off(EventBus *b, int id) {
    if (id >= 0 && id < MAX_SUBS) b->subs[id].used = 0;
}

void bus_emit(EventBus *b, const Event *ev) {
    for (int i = 0; i < MAX_SUBS; i++)
        if (b->subs[i].used && b->subs[i].type == ev->type)
            b->subs[i].fn(ev, b->subs[i].user_data);
}

/* ---- listeners ---- */
static void on_login(const Event *ev, void *ud) {
    (void)ud; printf("   [auth] %s logged in\n", ev->who);
}
static void on_logout(const Event *ev, void *ud) {
    (void)ud; printf("   [auth] %s logged out\n", ev->who);
}
static void on_message(const Event *ev, void *ud) {
    (void)ud; printf("   [chat] %s: %s\n", ev->who, ev->text);
}
/* a stats listener that counts messages via user_data */
static void count_messages(const Event *ev, void *ud) {
    (void)ev; int *n = ud; (*n)++;
}

int main(void) {
    EventBus bus = {0};
    int msg_count = 0;

    bus_on(&bus, EV_LOGIN,   on_login,   NULL);
    bus_on(&bus, EV_LOGOUT,  on_logout,  NULL);
    bus_on(&bus, EV_MESSAGE, on_message, NULL);
    int stats_id = bus_on(&bus, EV_MESSAGE, count_messages, &msg_count);

    Event events[] = {
        { EV_LOGIN,   "alice", NULL },
        { EV_MESSAGE, "alice", "hi everyone" },
        { EV_MESSAGE, "alice", "anyone here?" },
        { EV_LOGOUT,  "alice", NULL },
    };
    for (size_t i = 0; i < sizeof events / sizeof events[0]; i++)
        bus_emit(&bus, &events[i]);

    printf("\nMessages seen (via user_data counter): %d\n", msg_count);

    printf("\nUnregister the message listener and the stats counter, re-emit:\n");
    bus_off(&bus, stats_id);
    /* (on_message id was 2) */
    bus_off(&bus, 2);
    Event again = { EV_MESSAGE, "bob", "still anyone?" };
    bus_emit(&bus, &again);
    printf("   (no chat output -- message listeners were removed)\n");

    printf("\nA reusable bus: many listeners per event, payloads, per-listener\n");
    printf("context, and id-based removal. The core of pub/sub systems.\n");
    return 0;
}

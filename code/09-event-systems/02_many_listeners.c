/* 02_many_listeners.c
 *
 * The observer pattern: an event keeps a LIST of listeners and calls them all
 * when it fires. Adding a feature = registering another function; the
 * dispatcher never changes. You can also remove a listener by address.
 *
 * Build: gcc 02_many_listeners.c -o many && ./many
 */
#include <stdio.h>

#define MAX_LISTENERS 16

typedef void (*Listener)(void);

struct event {
    Listener listeners[MAX_LISTENERS];
    int count;
};

void on(struct event *e, Listener fn) {        /* register */
    if (e->count < MAX_LISTENERS)
        e->listeners[e->count++] = fn;
}

void off(struct event *e, Listener fn) {       /* unregister by address */
    for (int i = 0; i < e->count; i++) {
        if (e->listeners[i] == fn) {           /* compares addresses */
            e->listeners[i] = e->listeners[--e->count];
            return;
        }
    }
}

void emit(struct event *e) {                   /* dispatch to all */
    for (int i = 0; i < e->count; i++)
        e->listeners[i]();
}

/* Several independent reactions to the same "save" event. */
void write_file(void)    { printf("   - wrote file to disk\n"); }
void update_title(void)  { printf("   - updated title bar\n"); }
void log_action(void)    { printf("   - logged the save\n"); }

int main(void) {
    struct event on_save = { .count = 0 };

    on(&on_save, write_file);
    on(&on_save, update_title);
    on(&on_save, log_action);

    printf("User saves (3 listeners):\n");
    emit(&on_save);

    printf("\nRemove the logger, save again:\n");
    off(&on_save, log_action);
    emit(&on_save);

    printf("\nFeatures were added/removed by (un)registering functions.\n");
    printf("emit() -- the dispatcher -- never changed.\n");
    return 0;
}

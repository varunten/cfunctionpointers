/* 03_register_api.c
 *
 * Pattern C: register/unregister handlers. Registration returns an ID; you
 * unregister by ID (robust even if the same function is registered twice with
 * different user_data). This is the shape of GTK's g_signal_connect, etc.
 *
 * Build: gcc 03_register_api.c -o regapi && ./regapi
 */
#include <stdio.h>

#define MAX_HANDLERS 16

typedef void (*mylib_handler)(int event, void *user_data);

struct slot {
    int           used;
    mylib_handler fn;
    void         *user_data;
};

static struct slot g_slots[MAX_HANDLERS];

int mylib_on(mylib_handler fn, void *user_data) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (!g_slots[i].used) {
            g_slots[i].used = 1;
            g_slots[i].fn = fn;
            g_slots[i].user_data = user_data;
            return i;                 /* the registration id */
        }
    }
    return -1;                        /* table full */
}

void mylib_off(int id) {
    if (id >= 0 && id < MAX_HANDLERS)
        g_slots[id].used = 0;
}

void mylib_emit(int event) {
    for (int i = 0; i < MAX_HANDLERS; i++)
        if (g_slots[i].used)
            g_slots[i].fn(event, g_slots[i].user_data);
}

/* ---- user handlers ---- */
void handler_a(int ev, void *ud) { printf("   A got event %d (ctx=%s)\n", ev, (char*)ud); }
void handler_b(int ev, void *ud) { printf("   B got event %d (ctx=%s)\n", ev, (char*)ud); }

int main(void) {
    int ida = mylib_on(handler_a, "ctxA");
    int idb = mylib_on(handler_b, "ctxB");

    printf("Emit event 1 (both registered):\n");
    mylib_emit(1);

    printf("\nUnregister A (id=%d), emit event 2:\n", ida);
    mylib_off(ida);
    mylib_emit(2);

    (void)idb;
    printf("\nReturning an id makes removal unambiguous and robust.\n");
    return 0;
}

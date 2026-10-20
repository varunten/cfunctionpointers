/* 05_plugin_system.c  -- Capstone Project 5: a versioned plugin host.
 *
 * A plugin contract (struct of function pointers) with an ABI version, a
 * registry with lifecycle (init/run/cleanup), and a version check that rejects
 * incompatible plugins. Patterns: plugins (16), ops struct (14,15), registry
 * (09,11).
 *
 * Build: gcc 05_plugin_system.c -o plugins && ./plugins
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define PLUGIN_ABI_VERSION 1

/* The contract. New function pointers may be APPENDED in future versions. */
typedef struct {
    int          abi_version;
    const char  *name;
    int  (*init)(void);
    char (*transform)(char c);     /* per-character transform for this demo */
    void (*cleanup)(void);
} Plugin;

/* ---- plugin: uppercase ---- */
static int  up_init(void) { return 0; }
static char up_tx(char c) { return (char)toupper((unsigned char)c); }
static void up_clean(void) {}
static Plugin uppercase = { PLUGIN_ABI_VERSION, "uppercase", up_init, up_tx, up_clean };

/* ---- plugin: rot13 ---- */
static int  r13_init(void) { return 0; }
static char r13_tx(char c) {
    if (c >= 'a' && c <= 'z') return (char)('a' + (c - 'a' + 13) % 26);
    if (c >= 'A' && c <= 'Z') return (char)('A' + (c - 'A' + 13) % 26);
    return c;
}
static void r13_clean(void) {}
static Plugin rot13 = { PLUGIN_ABI_VERSION, "rot13", r13_init, r13_tx, r13_clean };

/* ---- plugin: stale (wrong ABI) ---- */
static char id_tx(char c) { return c; }
static Plugin legacy = { 0 /* old ABI */, "legacy", up_init, id_tx, up_clean };

/* ---- host registry ---- */
#define MAX_PLUGINS 16
static Plugin *registry[MAX_PLUGINS];
static int     count;

static int host_register(Plugin *p) {
    if (p->abi_version != PLUGIN_ABI_VERSION) {
        printf("REJECT '%s' (ABI %d, need %d)\n", p->name, p->abi_version, PLUGIN_ABI_VERSION);
        return -1;
    }
    if (count >= MAX_PLUGINS || p->init() != 0) return -1;
    registry[count++] = p;
    printf("ACCEPT '%s'\n", p->name);
    return 0;
}

static void host_apply(const char *text) {
    for (int i = 0; i < count; i++) {
        printf("[%s] ", registry[i]->name);
        for (const char *s = text; *s; s++)
            putchar(registry[i]->transform(*s));   /* through the contract */
        putchar('\n');
    }
}

static void host_shutdown(void) {
    for (int i = 0; i < count; i++) registry[i]->cleanup();
    count = 0;
}

int main(void) {
    printf("Loading plugins:\n");
    host_register(&uppercase);
    host_register(&rot13);
    host_register(&legacy);     /* rejected: incompatible ABI */

    printf("\nApplying accepted plugins to \"Hello, Plugins!\":\n");
    host_apply("Hello, Plugins!");

    host_shutdown();
    printf("\nThe host drove every plugin through the same contract, and the\n");
    printf("version check kept the incompatible one out. Extensibility, safely.\n");
    return 0;
}

/* 01_plugin_contract.c
 *
 * The core of every plugin system: a CONTRACT that is a struct of function
 * pointers. A plugin is an instance of that struct with the pointers filled in.
 * The host calls through the contract without knowing what any plugin does.
 *
 * Build: gcc 01_plugin_contract.c -o plug && ./plug
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* ---- the contract (shared by host and all plugins) ---- */
typedef struct {
    const char *name;
    int  (*init)(void);
    void (*run)(const char *input);
    void (*cleanup)(void);
} Plugin;

/* ---- plugin 1: uppercase ---- */
static int  up_init(void) { return 0; }
static void up_run(const char *in) {
    printf("   ");
    for (const char *p = in; *p; p++) putchar(toupper((unsigned char)*p));
    putchar('\n');
}
static void up_cleanup(void) {}

Plugin uppercase_plugin = { "uppercase", up_init, up_run, up_cleanup };

/* ---- plugin 2: reverse ---- */
static int  rev_init(void) { return 0; }
static void rev_run(const char *in) {
    size_t n = strlen(in);
    printf("   ");
    for (size_t i = n; i > 0; i--) putchar(in[i - 1]);
    putchar('\n');
}
static void rev_cleanup(void) {}

Plugin reverse_plugin = { "reverse", rev_init, rev_run, rev_cleanup };

/* ---- host: uses any plugin through the contract ---- */
void use_plugin(Plugin *p, const char *input) {
    printf("[plugin: %s]\n", p->name);
    if (p->init() != 0) { printf("   init failed\n"); return; }
    p->run(input);
    p->cleanup();
}

int main(void) {
    const char *text = "Hello Plugins";
    use_plugin(&uppercase_plugin, text);
    use_plugin(&reverse_plugin,   text);

    printf("\nThe host called init/run/cleanup through the contract, with no\n");
    printf("knowledge of what each plugin actually does. That's extensibility.\n");
    return 0;
}

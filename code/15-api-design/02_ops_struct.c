/* 02_ops_struct.c
 *
 * Pattern B: bundle several related callbacks into a config/ops struct the user
 * fills in. This is how SQLite/Lua allocator hooks and the Linux kernel's
 * file_operations are shaped. Extensible: add a field without changing any
 * function signature; old code just leaves new fields NULL.
 *
 * Build: gcc 02_ops_struct.c -o opsapi && ./opsapi
 */
#include <stdio.h>
#include <stdlib.h>

/* ---- "mylib" public API ---- */
typedef struct {
    void *(*alloc)(size_t size);          /* how to allocate */
    void  (*free)(void *ptr);             /* how to free */
    void  (*on_error)(const char *msg);   /* what to do on error */
} mylib_config;

static mylib_config g_cfg;

void mylib_init(const mylib_config *cfg) { g_cfg = *cfg; }

void mylib_run(int fail) {
    void *buf = g_cfg.alloc(64);
    if (!buf) { if (g_cfg.on_error) g_cfg.on_error("alloc failed"); return; }
    printf("   allocated a 64-byte buffer via the user's allocator\n");

    if (fail && g_cfg.on_error)
        g_cfg.on_error("simulated failure during run");

    g_cfg.free(buf);
    printf("   freed the buffer via the user's free\n");
}

/* ---- user-supplied implementations ---- */
static int alloc_count = 0;
void *my_alloc(size_t n) { alloc_count++; return malloc(n); }
void  my_free(void *p)   { free(p); }
void  my_error(const char *m) { printf("   [ERROR] %s\n", m); }

int main(void) {
    mylib_config cfg = {
        .alloc    = my_alloc,
        .free     = my_free,
        .on_error = my_error,
    };
    mylib_init(&cfg);

    printf("Run (success):\n");
    mylib_run(0);

    printf("\nRun (with error):\n");
    mylib_run(1);

    printf("\nThe library used our allocator (%d allocs) and error handler.\n",
           alloc_count);
    printf("All the policy lived in the config struct we passed in.\n");
    return 0;
}

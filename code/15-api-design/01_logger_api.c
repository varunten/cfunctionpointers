/* 01_logger_api.c
 *
 * Pattern A: a library exposes a SINGLE callback the user can set. Note the
 * three hallmarks of a good callback hook:
 *   - a typedef for readability
 *   - a void *user_data for caller context
 *   - NULL default meaning "feature off" (here: no logging)
 *
 * Build: gcc 01_logger_api.c -o logapi && ./logapi
 */
#include <stdio.h>

/* ---- "mylib" public API ---- */
typedef void (*mylib_log_fn)(const char *message, void *user_data);

static mylib_log_fn g_logger = NULL;     /* default: no logger installed */
static void        *g_logger_data = NULL;

void mylib_set_logger(mylib_log_fn logger, void *user_data) {
    g_logger = logger;
    g_logger_data = user_data;
}

/* internal: the library logs through whatever the user installed (if any) */
static void mylib_log(const char *msg) {
    if (g_logger)
        g_logger(msg, g_logger_data);
}

void mylib_do_something(void) {
    mylib_log("starting work");
    /* ... work ... */
    mylib_log("work complete");
}

/* ---- user code ---- */
void my_logger(const char *message, void *user_data) {
    const char *tag = user_data;          /* user_data carries our context */
    printf("[%s] %s\n", tag, message);
}

int main(void) {
    printf("Before installing a logger (NULL = silent):\n");
    mylib_do_something();                  /* no output: logging off */

    printf("\nAfter installing a logger:\n");
    mylib_set_logger(my_logger, "APP");
    mylib_do_something();                  /* now logs with our [APP] tag */

    return 0;
}

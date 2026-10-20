/* 01_command_dispatcher.c  -- Capstone Project 1: a mini command dispatcher.
 *
 * The skeleton of every shell/REPL: typed commands map to handler functions via
 * a dispatch table of { name, fn, help }. Patterns: dispatch table (11),
 * callbacks (07), data-driven help (15).
 *
 * This demo feeds a fixed script of commands so it needs no interactive input.
 *
 * Build: gcc 01_command_dispatcher.c -o dispatcher && ./dispatcher
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 8

/* A command handler returns 0 to continue, nonzero to stop the loop. */
typedef int (*CmdFn)(int argc, char **argv);

struct command {
    const char *name;
    CmdFn       fn;
    const char *help;
};

/* forward decl: help lists the table, so we need the table visible to it */
static struct command commands[];
static int NCMD;

static int cmd_help(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("commands:\n");
    for (int i = 0; i < NCMD; i++)
        printf("   %-6s - %s\n", commands[i].name, commands[i].help);
    return 0;
}

static int cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++)
        printf("%s%s", argv[i], i + 1 < argc ? " " : "");
    printf("\n");
    return 0;
}

static int cmd_add(int argc, char **argv) {
    long sum = 0;
    for (int i = 1; i < argc; i++)
        sum += strtol(argv[i], NULL, 10);
    printf("%ld\n", sum);
    return 0;
}

static int cmd_quit(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("bye\n");
    return 1;     /* nonzero stops the loop */
}

static struct command commands[] = {
    { "help", cmd_help, "list commands" },
    { "echo", cmd_echo, "print the arguments" },
    { "add",  cmd_add,  "sum integer arguments" },
    { "quit", cmd_quit, "exit" },
};
static int NCMD = (int)(sizeof commands / sizeof commands[0]);

static CmdFn lookup(const char *name) {
    for (int i = 0; i < NCMD; i++)
        if (strcmp(commands[i].name, name) == 0)
            return commands[i].fn;
    return NULL;
}

/* split a line into argv (destructive; uses the line's own storage) */
static int split(char *line, char **argv) {
    int argc = 0;
    char *tok = strtok(line, " \t\n");
    while (tok && argc < MAX_ARGS) {
        argv[argc++] = tok;
        tok = strtok(NULL, " \t\n");
    }
    return argc;
}

static int run_line(char *line) {
    char *argv[MAX_ARGS];
    int argc = split(line, argv);
    if (argc == 0) return 0;

    CmdFn fn = lookup(argv[0]);
    if (!fn) { printf("unknown command: %s (try 'help')\n", argv[0]); return 0; }
    return fn(argc, argv);
}

int main(void) {
    /* scripted "session" so the demo is non-interactive */
    const char *script[] = {
        "help",
        "echo hello world",
        "add 2 3 4 5",
        "dance now",
        "quit",
    };
    int n = (int)(sizeof script / sizeof script[0]);

    char buf[128];
    for (int i = 0; i < n; i++) {
        snprintf(buf, sizeof buf, "%s", script[i]);
        printf("$ %s\n", script[i]);
        if (run_line(buf))
            break;
    }

    printf("\nAdding a command = one table row + one function. The loop and the\n");
    printf("'help' command are data-driven and never change.\n");
    return 0;
}

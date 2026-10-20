/* 03_command_table.c
 *
 * A string-keyed dispatch table: { name, handler, help }. This is exactly how a
 * shell/REPL maps a typed command to a function. Selector is a string, so we
 * search the table (a hashmap would scale better for many commands -- the values
 * stored would still be function pointers).
 *
 * Build: gcc 03_command_table.c -o cmds && ./cmds
 */
#include <stdio.h>
#include <string.h>

typedef void (*CmdFn)(const char *args);

void cmd_help(const char *args);   /* forward decl: help lists the table */
void cmd_echo(const char *args) { printf("%s\n", args); }
void cmd_upper(const char *args) {
    for (const char *p = args; *p; p++)
        putchar(*p >= 'a' && *p <= 'z' ? *p - 32 : *p);
    putchar('\n');
}

struct command {
    const char *name;
    CmdFn       handler;
    const char *help;
};

static struct command commands[] = {
    { "help",  cmd_help,  "list available commands" },
    { "echo",  cmd_echo,  "print the arguments" },
    { "upper", cmd_upper, "print the arguments in upper case" },
};
static const int NCMD = (int)(sizeof commands / sizeof commands[0]);

void cmd_help(const char *args) {
    (void)args;
    printf("commands:\n");
    for (int i = 0; i < NCMD; i++)
        printf("   %-6s - %s\n", commands[i].name, commands[i].help);
}

/* look up a command by name; NULL if unknown */
CmdFn lookup(const char *name) {
    for (int i = 0; i < NCMD; i++)
        if (strcmp(commands[i].name, name) == 0)
            return commands[i].handler;
    return NULL;
}

void run(const char *name, const char *args) {
    CmdFn fn = lookup(name);
    printf("> %s %s\n", name, args);
    if (fn)
        fn(args);
    else
        printf("unknown command: %s (try 'help')\n", name);
}

int main(void) {
    run("help",  "");
    run("echo",  "hello world");
    run("upper", "make me loud");
    run("dance", "now");      /* unknown -> handled gracefully */
    return 0;
}

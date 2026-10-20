/* 02_plugin_registry.c
 *
 * A host keeps a REGISTRY of plugins and drives them all uniformly. This is
 * section 09's listener list + section 14's virtual dispatch + section 11's
 * dispatch table, combined: a list of contracts (structs of function pointers),
 * iterated and called.
 *
 * Build: gcc 02_plugin_registry.c -o preg && ./preg
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef struct {
    const char *name;
    int  (*init)(void);
    void (*run)(const char *input);
    void (*cleanup)(void);
} Plugin;

/* --- three tiny plugins --- */
static int  up_init(void){return 0;}
static void up_run(const char *s){ printf("   "); for(;*s;s++) putchar(toupper((unsigned char)*s)); putchar('\n'); }
static void up_clean(void){}
Plugin uppercase_plugin = { "uppercase", up_init, up_run, up_clean };

static int  rev_init(void){return 0;}
static void rev_run(const char *s){ size_t n=strlen(s); printf("   "); while(n) putchar(s[--n]); putchar('\n'); }
static void rev_clean(void){}
Plugin reverse_plugin = { "reverse", rev_init, rev_run, rev_clean };

static int  rot_init(void){return 0;}
static void rot_run(const char *s){
    printf("   ");
    for(;*s;s++){ char c=*s;
        if(c>='a'&&c<='z') c=(char)('a'+(c-'a'+13)%26);
        else if(c>='A'&&c<='Z') c=(char)('A'+(c-'A'+13)%26);
        putchar(c);
    }
    putchar('\n');
}
static void rot_clean(void){}
Plugin rot13_plugin = { "rot13", rot_init, rot_run, rot_clean };

/* --- host registry --- */
#define MAX_PLUGINS 32
static Plugin *registry[MAX_PLUGINS];
static int     plugin_count;

void register_plugin(Plugin *p) {
    if (plugin_count < MAX_PLUGINS && p->init() == 0) {
        registry[plugin_count++] = p;
        printf("registered plugin: %s\n", p->name);
    }
}

void run_all(const char *input) {
    printf("\nrunning all plugins on \"%s\":\n", input);
    for (int i = 0; i < plugin_count; i++) {
        printf("[%s]\n", registry[i]->name);
        registry[i]->run(input);          /* call each through the contract */
    }
}

void shutdown_all(void) {
    for (int i = 0; i < plugin_count; i++)
        registry[i]->cleanup();
    plugin_count = 0;
}

int main(void) {
    register_plugin(&uppercase_plugin);
    register_plugin(&reverse_plugin);
    register_plugin(&rot13_plugin);

    run_all("Hello World");

    shutdown_all();
    printf("\nAdding a feature = writing a plugin and registering it. The host\n");
    printf("loop never changes. Every series pattern meets here.\n");
    return 0;
}

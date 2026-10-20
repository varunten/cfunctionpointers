/* 03_versioned_contract.c
 *
 * Plugins are often compiled separately from the host. If the contract changes,
 * old plugins can crash the host. The fix: an ABI version field in the contract,
 * checked at load time. Mismatched plugins are rejected, not run.
 *
 * Build: gcc 03_versioned_contract.c -o pver && ./pver
 */
#include <stdio.h>

#define PLUGIN_ABI_VERSION 2

typedef struct {
    int          abi_version;     /* plugin sets this; host verifies it */
    const char  *name;
    void (*run)(void);
} Plugin;

void modern_run(void) { printf("   modern plugin running\n"); }
void old_run(void)    { printf("   old plugin running\n"); }

/* A current plugin: built against ABI v2. */
Plugin modern_plugin = { PLUGIN_ABI_VERSION, "modern", modern_run };

/* A stale plugin: built against ABI v1 (incompatible). */
Plugin old_plugin = { 1, "legacy", old_run };

/* Host: only loads plugins whose ABI matches. */
int load_plugin(Plugin *p) {
    if (p->abi_version != PLUGIN_ABI_VERSION) {
        printf("REJECT '%s': ABI %d, host needs %d\n",
               p->name, p->abi_version, PLUGIN_ABI_VERSION);
        return -1;
    }
    printf("ACCEPT '%s' (ABI %d)\n", p->name, p->abi_version);
    p->run();
    return 0;
}

int main(void) {
    load_plugin(&modern_plugin);   /* accepted and run */
    load_plugin(&old_plugin);      /* rejected -- avoids a crash */

    printf("\nThe version check is what lets a plugin system survive upgrades.\n");
    printf("Append new function pointers at the end and bump the version.\n");
    return 0;
}

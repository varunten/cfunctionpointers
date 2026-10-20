/* dynamic_loading_reference.c   (REFERENCE -- not built by the README's commands)
 *
 * This file documents the DYNAMIC plugin flow: loading code from a separate
 * shared library at runtime and obtaining function pointers into it. It is
 * OS-specific, so it is provided as a reference rather than a portable example.
 * The CONCEPT is identical to the static examples: get a struct of function
 * pointers, call through it. The only new part is HOW you obtain that struct --
 * from a file loaded at runtime.
 *
 * ------------------------------------------------------------------------
 * The contract (would live in a shared plugin.h used by host AND plugins):
 *
 *     typedef struct {
 *         const char *name;
 *         void (*run)(const char *input);
 *     } Plugin;
 *
 * ------------------------------------------------------------------------
 * THE PLUGIN (compiled to uppercase.so / uppercase.dll), exports a known symbol:
 *
 *     #include "plugin.h"
 *     static void up_run(const char *in) { ... }
 *     static Plugin self = { "uppercase", up_run };
 *
 *     // well-known entry point the host will look up by name:
 *     Plugin *get_plugin(void) { return &self; }
 *
 *     Build (Linux/macOS):  gcc -shared -fPIC uppercase.c -o uppercase.so
 *     Build (Windows):      gcc -shared uppercase.c -o uppercase.dll
 *
 * ------------------------------------------------------------------------
 * THE HOST (POSIX: dlopen/dlsym/dlclose, link with -ldl):
 *
 *     #include <dlfcn.h>
 *     #include "plugin.h"
 *
 *     int main(void) {
 *         // 1. LOAD the shared library at runtime
 *         void *handle = dlopen("./uppercase.so", RTLD_NOW);
 *         if (!handle) { fprintf(stderr, "%s\n", dlerror()); return 1; }
 *
 *         // 2. RESOLVE the entry-point symbol -> a FUNCTION POINTER into the .so
 *         //    (the cast dance avoids a strict-aliasing warning for func ptrs)
 *         Plugin *(*get_plugin)(void);
 *         *(void **)(&get_plugin) = dlsym(handle, "get_plugin");
 *         if (!get_plugin) { fprintf(stderr, "no get_plugin\n"); return 1; }
 *
 *         // 3. HANDSHAKE: call it to receive the contract struct
 *         Plugin *p = get_plugin();
 *
 *         // 4. USE: call through the contract -- into code we never linked!
 *         p->run("hello dynamic world");
 *
 *         // 5. UNLOAD. After this, p and p->run DANGLE -- never call them again.
 *         dlclose(handle);
 *         return 0;
 *     }
 *
 * ------------------------------------------------------------------------
 * THE HOST (Windows: LoadLibrary/GetProcAddress/FreeLibrary):
 *
 *     #include <windows.h>
 *     HMODULE h = LoadLibrary("uppercase.dll");
 *     Plugin *(*get_plugin)(void) =
 *         (Plugin *(*)(void))GetProcAddress(h, "get_plugin");
 *     Plugin *p = get_plugin();
 *     p->run("hello");
 *     FreeLibrary(h);
 *
 * ------------------------------------------------------------------------
 * KEY TAKEAWAY
 *   dlsym / GetProcAddress RETURN A FUNCTION POINTER to code that was compiled
 *   separately and loaded at runtime. This is the ultimate proof of section 01:
 *   a function is just bytes at an address, and an address is an address --
 *   even one that entered the program seconds ago from a file on disk.
 *
 * This file intentionally has no main() to compile; it is documentation.
 */

# 16 — Plugin Architectures

> A plugin is "user code the program didn't know about at compile time,"
> discovered and loaded at runtime. The bridge between host and plugin is — you
> guessed it — a **table of function pointers**. This section shows how the
> patterns you've built (ops structs, registration, vtables) combine into real
> extensibility, both *within* a program and across dynamically loaded shared
> libraries.

---

## 1. What a plugin system is for

You want users (or your future self) to add features — new file formats, new
commands, new effects, new devices — **without modifying or recompiling the host
program**. The host defines a **contract**: "a plugin is something that provides
*these* functions." Anything satisfying the contract can plug in.

```text
   HOST (stable core)                      PLUGINS (added later)
   ------------------                      ---------------------
   defines a function-pointer contract     each provides those functions
   discovers & loads plugins               registers itself with the host
   calls plugin functions through the      runs when the host calls it
   contract, never knowing the details
```

Browsers (extensions), editors (VS Code), audio software (VST), games (mods),
and servers (Apache/nginx modules) all work this way. The contract is always
*function pointers grouped in a struct.*

---

## 2. The contract: a struct of function pointers

The heart of any plugin system is the **plugin interface** — a struct the host
defines and every plugin fills in. It's the ops struct from section 15, now used
as a *plugin's identity*:

```c
/* plugin.h -- the contract, shared by host and all plugins */
typedef struct {
    const char *name;
    int  (*init)(void);                 // called when the plugin loads
    void (*run)(const char *input);     // the plugin's main action
    void (*cleanup)(void);              // called when the plugin unloads
} Plugin;
```

A plugin is just an instance of this struct with the pointers filled in:

```c
/* uppercase_plugin.c */
static int  up_init(void)               { return 0; }
static void up_run(const char *input)   { /* ...print input uppercased... */ }
static void up_cleanup(void)            { }

Plugin uppercase_plugin = {
    .name = "uppercase",
    .init = up_init,
    .run  = up_run,
    .cleanup = up_cleanup,
};
```

```text
   Host                        Plugin (the struct)
   +----------------+          +--------------------------+
   | Plugin *p ----------------> name    = "uppercase"    |
   |                |          | init    = ----> up_init  |
   | p->init();     |--------->| run     = ----> up_run   |
   | p->run(text);  |          | cleanup = ----> up_cleanup|
   | p->cleanup();  |          +--------------------------+
   +----------------+
```

The host calls `p->init()`, `p->run(text)`, `p->cleanup()` — virtual dispatch
(section 14) by another name. The host has *no idea* what "uppercase" does; it
only knows the contract.

---

## 3. Two flavors: static (in-program) and dynamic (shared libraries)

### Static plugins (compiled in, registered at startup)

All plugins are compiled into the program, but the host treats them uniformly
through the contract and keeps them in a registry (an array/list of `Plugin *`).
This already gives you the architecture benefit (uniform, extensible code) even
without runtime loading. It's how many embedded systems and self-contained apps
do "plugins." We build this in code — it's pure function pointers, fully
portable.

```text
   registry: [ &uppercase_plugin, &reverse_plugin, &rot13_plugin ]
                     |                  |               |
              the host loops the registry and calls each through the contract
```

### Dynamic plugins (loaded at runtime from .so/.dll files)

The real extensibility: plugins are *separate compiled files* (`.so` on
Linux/macOS, `.dll` on Windows) the host loads at runtime, even ones written
*after* the host shipped. The OS gives you APIs to:

- **Load** a shared library: `dlopen` (POSIX) / `LoadLibrary` (Windows).
- **Find a symbol** (a function or variable) by name: `dlsym` / `GetProcAddress`
  — *this returns a function pointer!*
- **Unload**: `dlclose` / `FreeLibrary`.

```c
#include <dlfcn.h>   // POSIX

void *handle = dlopen("./uppercase.so", RTLD_NOW);     // load the .so
Plugin *(*get_plugin)(void) = dlsym(handle, "get_plugin"); // find entry point
Plugin *p = get_plugin();                               // get the contract struct
p->run("hello");                                        // use it
dlclose(handle);                                        // unload
```

> **`dlsym`/`GetProcAddress` literally hand you a function pointer to code in a
> file your program never linked against.** This is the ultimate demonstration of
> section 01's lesson: functions are just bytes at addresses, and an address is
> an address — even one in a library loaded seconds ago. Everything in this
> series culminates here.

Because dynamic loading is OS-specific (and MinGW/Windows vs POSIX differ), our
runnable code uses the **static** flavor (portable, same idea), and this README
plus a non-compiled reference snippet show the dynamic version. The *concept* is
identical: get a struct of function pointers, call through it.

---

## 4. Step-by-step: the dynamic plugin lifecycle

```text
   1. DISCOVER: host scans a plugins/ directory, finds uppercase.so.

   2. LOAD: dlopen("uppercase.so") -> the OS maps the library's code into memory
            (its functions now live in the address space, at real addresses).

   3. RESOLVE: dlsym(handle, "get_plugin") -> returns a function pointer to the
            plugin's well-known entry point.

   4. HANDSHAKE: host calls get_plugin() -> receives a Plugin* (the contract
            struct, full of function pointers into the loaded code).

   5. INIT: host calls p->init().

   6. USE: host calls p->run(...) whenever appropriate -- indirect calls into
            code that didn't exist when the host was compiled.

   7. UNLOAD: host calls p->cleanup(), then dlclose(handle) -> the OS unmaps the
            code. (Now any stored pointer into it DANGLES -- see pitfalls.)
```

Steps 3–6 are the whole point: a string name becomes a function pointer (`dlsym`)
becomes a struct of function pointers (`get_plugin`) becomes behavior (`p->run`).

---

## 5. The entry-point convention

How does the host find the plugin in a freshly loaded library? By agreeing on a
**well-known symbol name** every plugin must export. Common conventions:

- A function with a fixed name returning the contract:
  `Plugin *get_plugin(void);` — the host `dlsym`s `"get_plugin"`.
- A fixed-name *exported struct*: the plugin declares `Plugin plugin_info = {…};`
  and the host `dlsym`s `"plugin_info"`.
- A registration callback: the host passes the plugin a `register_with_host(...)`
  function the plugin calls to announce its capabilities (inversion of control
  again — section 07).

All three are "find a known symbol, get a function pointer / struct of them." The
registration-callback style is especially common because it lets one library
register *several* plugins and pass extra info.

---

## 6. Versioning the contract (critical for plugins)

Plugins are often compiled separately from the host, sometimes by third parties,
sometimes against an older host. If you change the `Plugin` struct, old plugins
break — possibly crashing the host. So real plugin systems put a **version
number** (an ABI version) in the contract and check it on load:

```c
#define PLUGIN_ABI_VERSION 2

typedef struct {
    int          abi_version;     // plugin sets this; host verifies it
    const char  *name;
    int  (*init)(void);
    void (*run)(const char *);
    void (*cleanup)(void);
} Plugin;

/* host, after loading: */
if (p->abi_version != PLUGIN_ABI_VERSION) {
    fprintf(stderr, "plugin '%s' has incompatible ABI %d (need %d)\n",
            p->name, p->abi_version, PLUGIN_ABI_VERSION);
    /* refuse to use it */
}
```

This is the "append-only struct + version field" advice from section 15, now
load-bearing: it's the difference between a plugin system that survives upgrades
and one that crashes mysteriously. Append new function pointers at the *end* and
bump the version; old plugins (lower version) are rejected or run in a
compatibility mode.

---

## 7. The host's side: a plugin registry

The host keeps a collection of loaded plugins and treats them uniformly:

```c
#define MAX_PLUGINS 32
static Plugin *registry[MAX_PLUGINS];
static int     plugin_count;

void register_plugin(Plugin *p) {
    if (plugin_count < MAX_PLUGINS && p->init() == 0)
        registry[plugin_count++] = p;
}

void run_all(const char *input) {
    for (int i = 0; i < plugin_count; i++)
        registry[i]->run(input);     // call each plugin through the contract
}
```

This is section 09's listener list and section 11's dispatch table, wearing a
plugin hat. *Everything connects:* a registry of contracts is a list of structs
of function pointers, iterated and called. Once you see that, plugin systems hold
no mystery.

---

## 8. Pitfalls unique to plugins

| Pitfall | Consequence | Mitigation |
|---|---|---|
| ABI mismatch | Crash / garbage on call | Version field, checked on load |
| Calling into an unloaded library | Dangling function pointer → crash | Never keep pointers after `dlclose`; clear the registry first |
| Plugin's `user_data`/state freed early | Use-after-free | Define ownership (section 15) clearly |
| Symbol not found | `dlsym` returns NULL | Check the return before calling! |
| Plugin crashes the host | Whole app dies (shared address space) | Validate inputs; advanced systems isolate plugins in processes |
| Name clashes between plugins | Wrong symbol resolved | Use unique names / load with local scope (`RTLD_LOCAL`) |

The "dangling after unload" one is the function-pointer version of use-after-free
and is the most insidious plugin bug. A function pointer is only as alive as the
code it points to — and `dlclose` can take that code away.

---

## 9. Where you'll meet this

- **Editors/IDEs:** VS Code extensions, Vim/Neovim plugins, Emacs packages.
- **Browsers:** extensions, and historically NPAPI plugins.
- **Audio/video:** VST/AU audio plugins, GStreamer/ffmpeg codecs.
- **Servers:** Apache modules, nginx modules, PostgreSQL extensions.
- **Games:** mod systems, scripting engines.
- **OS kernels:** loadable kernel modules (Linux `.ko`) — plugins for the
  *kernel*, registering via structs of function pointers (`file_operations`,
  driver ops).

In every case, the load mechanism varies but the core is identical: **discover
code at runtime, obtain a struct of function pointers, call through it.**

---

## 10. What to take into section 17

- A **plugin** is runtime-discovered code that satisfies a **contract** — a
  **struct of function pointers** the host defines.
- **Static** plugins (compiled in, registered) and **dynamic** plugins
  (`dlopen`/`LoadLibrary` + `dlsym`/`GetProcAddress`) share the same concept;
  dynamic loading returns **function pointers to code linked at runtime**.
- A well-known **entry-point symbol** bridges host and plugin; a **version
  field** keeps the contract safe across upgrades.
- The host keeps a **registry** (list of contracts) and calls each uniformly —
  dispatch tables and listener lists, reborn.
- Beware **dangling pointers after unload** and **ABI mismatches**.

You now have the full toolkit. The last two sections are about *fluency*:
reading real function-pointer-heavy code (17), and building complete projects
that exercise everything (18).

➡️ Continue to **`theory/17-reading-real-code/`**, after running
**`code/16-plugin-architectures/`**.

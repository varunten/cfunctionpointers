# 15 — API Design with Function Pointers

> You now understand function pointers mechanically and can build callbacks,
> dispatch tables, and vtables. This section is about *taste*: how to **design**
> a library API that exposes function pointers cleanly, so users can extend your
> code without you anticipating their needs. We cover the standard patterns,
> the all-important questions of **ownership and lifetime**, and the trade-offs
> real library authors weigh.

---

## 1. Why libraries expose function pointers

A library is most valuable when it can do things its author never imagined. A
logging library doesn't know *where* you want logs to go (console? file?
network?). A sort routine doesn't know *how* you order your data. A GUI toolkit
doesn't know *what* a button should do. The clean way to leave those decisions to
the user is a function pointer: "tell me the behavior, I'll call it at the right
time." This is the callback/inversion-of-control idea (section 07), now viewed
from the *library author's* side.

```text
   LIBRARY (general mechanism)        USER (specific policy)
   --------------------------         ----------------------
   "I handle WHEN and HOW."           "I supply WHAT."
   exposes a function-pointer hook ->  registers a callback
```

---

## 2. Pattern A — the single callback (the logger)

The simplest hook: one function pointer the user sets, which the library calls.

```c
/* mylib.h -- public header */
typedef void (*mylib_log_fn)(const char *message, void *user_data);

void mylib_set_logger(mylib_log_fn logger, void *user_data);
```

```c
/* mylib.c -- implementation */
static mylib_log_fn g_logger = NULL;
static void        *g_logger_data = NULL;

void mylib_set_logger(mylib_log_fn logger, void *user_data) {
    g_logger = logger;
    g_logger_data = user_data;
}

static void log_msg(const char *msg) {
    if (g_logger)                       /* optional: NULL = no logging */
        g_logger(msg, g_logger_data);
}
```

Note the design choices already present: a **`typedef`** for readability
(section 06), a **`void *user_data`** for caller context (section 07), and a
**NULL default** meaning "feature off." These three appear in nearly every good
callback API.

---

## 3. Pattern B — the struct of callbacks (the "ops" / config struct)

When a library needs *several* related hooks, don't pass five separate function
pointers — bundle them in a struct the user fills in. This is the **operations
struct** (a.k.a. "vtable for your API," "config struct," "interface").

```c
typedef struct {
    void *(*alloc)(size_t size);          // how to allocate
    void  (*free)(void *ptr);             // how to free
    void  (*on_error)(const char *msg);   // what to do on error
} mylib_config;

void mylib_init(const mylib_config *cfg);
```

The user fills in what they care about and passes the struct:

```c
mylib_config cfg = {
    .alloc    = my_alloc,
    .free     = my_free,
    .on_error = my_error_handler,
};
mylib_init(&cfg);
```

```text
   user-built config struct           library stores/uses it
   +-----------------------+          +------------------------+
   | alloc    = my_alloc   |          | calls cfg->alloc(...)  |
   | free     = my_free    |--------->| calls cfg->free(...)   |
   | on_error = my_error   |          | calls cfg->on_error()  |
   +-----------------------+          +------------------------+
```

This is exactly how SQLite's VFS, allocator hooks, the Linux kernel's
`file_operations`, and `libcurl`'s option system are shaped. Advantages over
loose parameters: extensible (add a field without changing the function
signature — old code leaves it zero/NULL), self-documenting, and groupable. It's
the same idea as a vtable (section 14), applied as a *configuration* surface.

---

## 4. Pattern C — register/unregister (the event API)

For "many subscribers" hooks (section 09), expose registration:

```c
typedef void (*mylib_handler)(int event, void *user_data);

int  mylib_on(int event_type, mylib_handler fn, void *user_data); // returns an id
void mylib_off(int registration_id);                              // remove by id
```

Returning an **id/handle** (rather than requiring the user to pass the same
function pointer back to unregister) is a robust touch — it handles the case
where the same function is registered multiple times with different `user_data`.

---

## 5. The hard part: ownership and lifetime

This is where APIs live or die. When you store a callback to call **later**
(asynchronously), two things must outlive the moment of registration:

1. The **function** itself — almost always fine (functions live in the text
   segment forever, section 01). The exception is dynamically loaded plugins
   (section 16): if you `dlclose` the library, its functions vanish and a stored
   pointer dangles.
2. The **`user_data`** it points to — *this* is the classic bug. If `user_data`
   points at a stack variable that goes out of scope, or heap memory that gets
   freed, calling the callback later reads freed/garbage memory.

```text
   register:  mylib_on(EV, cb, &local);   // &local points into THIS stack frame
   ...function returns, 'local' is gone...
   later:     event fires -> cb(EV, &local)  // DANGLING -- undefined behavior!
```

> **The golden rule:** whoever supplies `user_data` must guarantee it stays valid
> until the callback can no longer be called (i.e., until unregistration or
> library shutdown). Document this explicitly in your API.

Strategies an API can offer:

- **Caller-owns (most common):** "the data you pass must remain valid until you
  call `mylib_off`." Simple, zero-cost, but burdens the caller. Document loudly.
- **Library-copies:** the library copies the user data (needs to know its size,
  or take ownership). Safer for the caller, but the API must manage the memory.
- **Reference counting / destroy callback:** the API takes a
  `void (*destroy)(void *user_data)` it calls when it's done with the data, so
  the caller can free it at the right time. (GLib's `GDestroyNotify` does
  exactly this.)

Choosing among these is a real design decision with real trade-offs — there's no
universally right answer, only what fits your library's contract.

---

## 6. Designing the callback signature

A few rules of thumb that separate pleasant APIs from painful ones:

- **Always include a `void *user_data`.** Without it, callbacks can't carry
  context and users resort to ugly globals. This is the single most common
  beginner-API omission.
- **Pass enough context in the arguments.** The callback should be able to do its
  job from its parameters; don't force it to query global state.
- **Use `const` where you won't modify.** `const char *message` tells the user
  "I won't change this, and you shouldn't keep the pointer past the call unless I
  say so."
- **Define the return value's meaning.** Does returning nonzero stop iteration?
  Indicate an error? Document it (section 07's stoppable pattern).
- **Specify when and how often** the callback is invoked, and **from which
  thread** (huge for async APIs). "Called once per item, on the calling thread,
  before `foo` returns" vs "called later, possibly from a background thread" are
  worlds apart for the user.
- **Hide the type behind a `typedef`.** Users should never type
  `void (*)(const char *, void *)` themselves.

---

## 7. Case studies (how real libraries do it)

- **A logging library** (`mylib_set_logger`): single callback, `user_data`,
  NULL = silent. Pattern A.
- **A networking library** (`libcurl`): a huge `setopt` system where many options
  *are* callbacks — `CURLOPT_WRITEFUNCTION` (where to put received bytes) with
  `CURLOPT_WRITEDATA` (the `user_data`). Note the deliberate pairing of a
  function pointer with its data pointer — a recurring API idiom.
- **A GUI toolkit** (GTK): `g_signal_connect(widget, "clicked", callback,
  user_data)` — the register pattern (C) with a `GClosureNotify` destroy hook for
  lifetime (the ref-counting strategy from section 5).
- **An allocator-pluggable library** (SQLite, Lua): a struct/config of `alloc`/
  `realloc`/`free` function pointers — Pattern B. Lua's `lua_Alloc` is a single
  allocation callback covering all three operations.
- **An OS-abstraction layer** (SDL, SQLite VFS): a big struct of function
  pointers abstracting the platform — Pattern B at scale.

Reading any of these headers now should feel familiar: they're the patterns above
with domain-specific names. Section 17 practices exactly this kind of reading.

---

## 8. Versioning and ABI: a pro consideration

Once your function-pointer API ships, *changing* it can break users. Two tips
seasoned authors use:

- **Add, don't reorder.** Append new fields to a config/ops struct; never reorder
  or remove existing ones (it breaks compiled callers). New code sets the new
  field; old code leaves it zero/NULL, which you treat as "default."
- **Include a version or size field.** Start config structs with a `version` or
  `struct_size` field so the library can tell which fields are present. This lets
  the API grow without breaking old binaries.

You don't need this for a class project, but recognizing it explains the
`size_t struct_size;`-as-first-field pattern you'll see in mature C APIs.

---

## 9. Anti-patterns to avoid

| Anti-pattern | Why it's bad | Better |
|---|---|---|
| No `user_data` | Callbacks can't carry context → forces globals | Always pass `void *user_data` |
| Many loose function-pointer params | Hard to read, hard to extend | Bundle in a config/ops struct |
| Undocumented lifetime | Dangling `user_data`, crashes | State the ownership contract explicitly |
| Undocumented thread/timing | Data races, surprises | Specify when/where the callback runs |
| Exposing raw `void(*)(...)` types | Ugly, error-prone for users | Hide behind a `typedef` |
| Reordering struct fields later | Breaks compiled users (ABI break) | Append-only + version/size field |

---

## 10. What to take into section 16

- Libraries expose function pointers to let users supply **policy** while the
  library owns **mechanism**.
- Core patterns: **single callback** (A), **struct of callbacks / ops** (B),
  **register/unregister** (C).
- Always provide **`void *user_data`**, hide types behind **`typedef`**, and use
  **`const`** appropriately.
- The crucial design axis is **ownership & lifetime** of `user_data` (caller-owns
  vs library-copies vs destroy-callback) — document it.
- For longevity: **append-only structs** with a version/size field.

A plugin system is an API like this taken to its logical extreme — where the
"user code" is an entirely separate module discovered at runtime. That's
section 16.

➡️ Continue to **`theory/16-plugin-architectures/`**, after running
**`code/15-api-design/`**.

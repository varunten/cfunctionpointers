# 17 — Reading Real Code

> Knowing how to *write* function pointers is half the battle. The other half is
> *reading* them in the wild — in standard library headers, system APIs, and
> sprawling codebases where they appear without explanation. This section is a
> field guide: the recurring patterns, how to decode scary declarations fast, and
> annotated tours of real-world function-pointer code you'll actually encounter.

---

## 1. The reading mindset

When you hit function-pointer code, don't try to understand the whole file. Ask
three questions:

1. **Who stores the pointer?** (a struct field, a global, a parameter, an array)
2. **Who sets it?** (the user via an API, a constructor, a table initializer)
3. **Who calls it, and when?** (a loop, an event, a `qsort`-style algorithm)

These map to the patterns you've learned: callback (07), dispatch table (11),
vtable (14), plugin (16). Almost all real code is one of these. Pattern-matching
is faster than reading line by line.

```text
   stored where?   set by whom?    called when?     => which pattern
   -------------    ------------    ------------        --------------
   a parameter      the caller      during the call     callback (sync)
   a global/field   an API setter   later, on event     callback (async)/event
   an array slot    a table init    by index/opcode     dispatch/jump table
   a struct field   a constructor   obj->fn(obj,...)    vtable / polymorphism
   from dlsym       runtime load    through a contract  plugin
```

---

## 2. Decoding scary declarations fast

Reach for these tools, in order:

1. **The Right-Left rule** (section 05) for any raw declaration.
2. **Mentally insert a `typedef`** to break a monster into pieces.
3. **`cdecl`** — the classic "C gibberish ↔ English" translator (there's an
   online version, `cdecl.org`, and a command-line `cdecl`). Type a declaration,
   get plain English. Invaluable for the truly gnarly ones.
4. **Read the *usage*** — find where the thing is *called*; the call site often
   reveals the shape faster than the declaration.

Worked example — the real POSIX `signal`:

```c
void (*signal(int sig, void (*handler)(int)))(int);
```

Insert the obvious typedef and it dissolves:

```c
typedef void (*sighandler_t)(int);          // a handler: takes int, returns void
sighandler_t signal(int sig, sighandler_t handler);
```

> "`signal` takes a signal number and a handler, and returns the previous
> handler." When you meet a horror declaration, *this* is the move: find the
> repeated function-pointer shape, name it, and the rest reads like English.

---

## 3. Tour: `qsort` (the standard library)

You studied this in section 08; here's how to *recognize* it cold:

```c
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));
```

Reading checklist:
- **Stored?** `compar` is a parameter — a *synchronous* callback.
- **Set?** By you, at the call.
- **Called?** Inside `qsort`, many times, during the call.
- **Shape?** `int (*)(const void *, const void *)` → "compare two unknown things,
  return ordering." The `const void *` screams "generic over type." The `int`
  return + the name `compar` says "comparator."

Pattern: **callback (comparator)**. The moment you see `(const void *,
const void *) -> int`, think "qsort-style comparison." `bsearch`, and many
`tree`/`list` libraries use the identical shape.

---

## 4. Tour: `pthread_create` (threads)

```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
```

Reading checklist:
- **`start_routine`** is `void *(*)(void *)` → "a function taking a `void *`,
  returning a `void *`." It's the function the new thread will run.
- **`arg`** is the `void *user_data` (section 07) passed to that function — how
  you give the thread its starting data.
- **Called?** *Asynchronously*, by the new thread, possibly long after
  `pthread_create` returns → **lifetime matters** (section 15): `arg` must stay
  valid until the thread reads it.

Pattern: **callback (async) + user_data**. Recognize the `void *(*)(void *)` +
`void *arg` pairing — it's the universal "run this function, with this data, over
there" idiom. `atexit`, `signal`, timer APIs, and thread pools all rhyme with it.

---

## 5. Tour: the Linux kernel `file_operations` (a vtable / ops struct)

Every Linux device driver fills in a struct like this (abridged):

```c
struct file_operations {
    struct module *owner;
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *);
    int     (*open) (struct inode *, struct file *);
    int     (*release)(struct inode *, struct file *);
    long    (*unlocked_ioctl)(struct file *, unsigned int, unsigned long);
    /* ...dozens more... */
};
```

Reading checklist:
- A **struct full of function pointers** → an **ops struct / vtable** (sections
  14–15). It's the *interface* every "file-like" object implements.
- Each method takes a **`struct file *`** (or `inode *`) first — that's `self`
  (section 13). The kernel calls `f->f_op->read(f, ...)` — virtual dispatch.
- A driver provides its own functions for the slots it supports and leaves the
  rest `NULL`; the kernel checks for `NULL` before calling (optional methods).

Pattern: **vtable / polymorphism**. The whole Linux VFS (virtual filesystem) is
"call the function pointer in this object's ops struct" — exactly section 14.
`struct inode_operations`, `struct net_device_ops`, driver `*_ops` structs: all
the same shape. Once you see one, you can read them all.

---

## 6. Tour: a networking library (`libcurl`-style options)

```c
typedef size_t (*write_callback)(char *ptr, size_t size, size_t nmemb,
                                 void *userdata);

curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
curl_easy_setopt(handle, CURLOPT_WRITEDATA,     &my_buffer);
```

Reading checklist:
- A **callback** (`write_callback`) that receives chunks of downloaded data.
- Notice the **deliberate pairing**: `CURLOPT_WRITEFUNCTION` (the function) with
  `CURLOPT_WRITEDATA` (its `user_data`). When you see a "...FUNCTION" option,
  hunt for its matching "...DATA" option — that's the context channel
  (section 15).
- **Called?** During `curl_easy_perform`, once per received chunk → synchronous
  during transfer.

Pattern: **callback + user_data**, exposed through an options/setopt API
(section 15, pattern A scaled up). Many C libraries use this "set a function
option, set its data option" duo.

---

## 7. Tour: embedded / state machines and interrupt vectors

Embedded code leans hard on function pointers for two reasons: tight control and
small code size.

```c
/* a state machine (section 10) */
typedef void (*StateFn)(Context *ctx);
StateFn current_state;
/* ... main loop: current_state(&ctx); ... */

/* an interrupt vector table -- an array of function pointers the HARDWARE
   indexes when an interrupt fires (a jump table baked into the CPU) */
typedef void (*ISR)(void);
ISR vector_table[NUM_INTERRUPTS] = {
    [TIMER_IRQ] = on_timer,
    [UART_IRQ]  = on_uart_rx,
    /* ... */
};
```

Reading checklist:
- A single function pointer reassigned over time → **state machine** (section
  10).
- An **array of function pointers indexed by an event/interrupt number** →
  **jump/dispatch table** (sections 11–12). The interrupt vector table is the
  CPU's own jump table: when interrupt N fires, the hardware jumps to
  `vector_table[N]`. It's section 12, in silicon.

Pattern: **state machine + jump table.** Embedded and OS code is where you'll see
function pointers at their most load-bearing.

---

## 8. Tour: operating systems (syscall table, scheduler hooks)

- **The syscall table** is an array of function pointers indexed by syscall
  number (`sys_call_table[nr](...)`). `read`, `write`, `open` — each syscall
  number maps to a kernel function. Pure **dispatch table** (section 11/12).
- **Schedulers, allocators, and subsystems** expose ops structs so policies can
  be swapped (e.g. different scheduling classes). **vtable / plugin** pattern.

You won't read kernel source on day one, but knowing "the syscall table is just
an array of function pointers" makes a huge system suddenly approachable.

---

## 9. A practical decoding workflow

When you open an unfamiliar function-pointer-heavy file:

1. **Grep for `(*` and `typedef ... (*`** — find the function pointer types and
   any typedefs. The typedefs are your glossary.
2. **Find the structs of function pointers** — those are interfaces/vtables/ops.
   Note the first parameter (usually `self`).
3. **Find the arrays of function pointers** — those are dispatch/jump tables.
   Note what indexes them.
4. **Find where pointers are *assigned*** — that tells you the concrete
   implementations and who provides them.
5. **Find where they're *called*** — `->fn(`, `table[i](`, `cb(` — that's the
   behavior, and *when*.
6. **Map each to a pattern** (callback / table / vtable / plugin). Now you
   understand the architecture, not just the syntax.

This workflow turns "what is all this `(*)()` noise" into "ah, this is an ops
struct with a dispatch table for commands and a couple of async callbacks."

---

## 10. What to take into section 18

- Read function pointers by asking **who stores / sets / calls** them, then
  **match to a pattern** (callback, dispatch/jump table, vtable, plugin).
- Decode hard declarations with the **Right-Left rule**, a **mental typedef**,
  `cdecl`, or by reading the **call site**.
- Recognize the canonical shapes: `(const void*,const void*)->int` (comparator),
  `void*(*)(void*)`+`void*arg` (thread/callback+data), **structs of function
  pointers** (vtable/ops: kernel `file_operations`, libcurl), **arrays indexed by
  a number** (syscall/jump/interrupt tables).
- Real systems are these few patterns repeated — once you see them, big
  codebases stop being scary.

The capstone projects in section 18 pull *all* of this together: you'll build
complete programs that are, themselves, exactly the kind of code this section
taught you to read.

➡️ Continue to **`theory/18-capstone-projects/`**, after running
**`code/17-reading-real-code/`**.

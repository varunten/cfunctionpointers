# 06 — Typedefs

> Section 05 taught you to *read* raw function pointer declarations so no
> codebase intimidates you. This section teaches you to *write* clean code so you
> never inflict that syntax on yourself or your teammates. The tool is
> `typedef`, and it transforms function-pointer code from cryptic to obvious.

---

## 1. The problem typedefs solve

Raw function pointer types are verbose and they get repeated. Watch how the same
ugly type appears again and again:

```c
void register_callback(int (*cb)(int, int));
int  (*get_callback(void))(int, int);          // returns a function pointer -- yuck
int  (*callbacks[10])(int, int);               // an array of them
struct handler { int (*fn)(int, int); };       // and inside a struct
```

Four places, four spellings of "pointer to function taking (int,int) returning
int", each slightly differently shaped. If the signature ever changes, you edit
all four. And the `get_callback` line — a function returning a function pointer —
is borderline unreadable.

---

## 2. The fix: name the type once

`typedef` creates an **alias** for a type. For function pointers it's
transformative:

```c
typedef int (*BinaryOp)(int, int);
```

Read that with the Right-Left rule from section 05, but with `typedef` in front
it means: "**`BinaryOp` is now a name for** the type 'pointer to function taking
(int,int) returning int'." From now on, `BinaryOp` *is* that type.

Now rewrite the four ugly lines:

```c
void     register_callback(BinaryOp cb);     // clear
BinaryOp get_callback(void);                 // a function returning a BinaryOp -- obvious!
BinaryOp callbacks[10];                       // an array of BinaryOp
struct handler { BinaryOp fn; };              // a BinaryOp field
```

Every usage is now readable, consistent, and changing the signature means
editing **one** line — the `typedef`.

```text
   BEFORE                                AFTER (typedef BinaryOp once)
   ------                                -----------------------------
   int (*cb)(int,int)                    BinaryOp cb
   int (*get(void))(int,int)             BinaryOp get(void)
   int (*arr[10])(int,int)               BinaryOp arr[10]
   struct{ int (*fn)(int,int); }         struct{ BinaryOp fn; }
```

---

## 3. Anatomy of a function pointer typedef

```c
typedef int (*BinaryOp)(int, int);
//      ^^^  ^^^^^^^^^^  ^^^^^^^^^^
//      |    |           |
//      |    |           +-- parameters the function takes
//      |    +-------------- (*Name) : the alias being defined, as a pointer
//      +------------------- return type of the function
```

The structure is *identical* to declaring a variable named `BinaryOp`, with
`typedef` glued on the front. In fact, a reliable way to write one:

1. Write the variable declaration you'd use: `int (*BinaryOp)(int, int);`
2. Put `typedef` in front: `typedef int (*BinaryOp)(int, int);`
3. Now `BinaryOp` is a *type*, not a variable.

That's the whole trick. The name where the *variable* would have gone becomes
the *type name*.

---

## 4. Two styles: pointer-in-typedef vs function-type typedef

There are two conventions, and you'll meet both.

### Style A — the typedef *is* a pointer (most common)

```c
typedef int (*BinaryOp)(int, int);

BinaryOp op = add;     // op is already a pointer; no * needed
int r = op(2, 3);
```

The `*` lives inside the typedef, so `BinaryOp` is "pointer to function." Clean
to use.

### Style B — the typedef is the *function type* (pointer added at use)

```c
typedef int BinaryOpFn(int, int);   // note: NO * -- this names a FUNCTION type

BinaryOpFn *op = add;   // you add the * yourself when declaring a pointer
int r = op(2, 3);
```

Here `BinaryOpFn` names the *function* type itself, and you write `BinaryOpFn *`
to get a pointer. Some style guides (and parts of the Linux kernel) prefer this
because the `*` at the use-site makes "this is a pointer" visible.

```text
   Style A: typedef int (*BinaryOp)(int,int);   ->  BinaryOp  op;   (op is a pointer)
   Style B: typedef int  BinaryOpFn(int,int);   ->  BinaryOpFn *op; (you add the *)
```

Both are valid. Style A is the most widespread; pick one and be consistent.
This series uses Style A unless noted.

---

## 5. Naming conventions that help readers

A good typedef name signals intent. Common patterns:

- `BinaryOp`, `Comparator`, `Predicate` — describe the *role*.
- `Callback`, `EventHandler`, `Visitor` — describe the *usage pattern*.
- Suffix `_fn`, `_t`, or `_cb` in C-style code: `compare_fn`, `sort_cmp_t`,
  `log_cb`.

The goal: at the call site, a reader should understand *what kind of behavior*
the pointer represents without re-deriving the signature.

```c
typedef int  (*Comparator)(const void *, const void *);  // "compares two things"
typedef void (*EventHandler)(int event_id);              // "handles an event"
typedef int  (*Predicate)(int value);                    // "yes/no test on a value"
```

---

## 6. Where typedefs shine: the `signal` nightmare, cured

Recall the horror from section 05:

```c
void (*signal(int, void (*)(int)))(int);
```

Nobody should have to parse that. With typedefs:

```c
typedef void (*SignalHandler)(int);          // a handler: takes int, returns void

SignalHandler signal(int signum, SignalHandler handler);
```

Now it reads in plain English: "`signal` takes a signal number and a
`SignalHandler`, and returns a `SignalHandler`." This is *exactly* why the real
POSIX headers use a typedef (`sig_t` / `__sighandler_t`) for this. Typedefs
aren't just nicer — for some declarations they're practically mandatory for
sanity.

---

## 7. Typedefs in API design (a preview of section 15)

When you publish a library, the typedef becomes part of your **public contract**.
Users program against the *name*, not the raw signature:

```c
/* mylib.h */
typedef void (*mylib_log_fn)(const char *message);

void mylib_set_logger(mylib_log_fn logger);
```

Users write:

```c
void my_logger(const char *m) { fprintf(stderr, "[log] %s\n", m); }
...
mylib_set_logger(my_logger);
```

They never wrestle with `void (*)(const char *)`. The typedef documents intent,
keeps call sites clean, and gives you a single place to evolve the type. Every
serious C library does this.

---

## 8. A subtle caution: typedefs hide the pointer

The one downside of Style A: because the `*` is hidden, a reader might not
realize `BinaryOp` is a pointer.

```c
typedef int (*BinaryOp)(int, int);
BinaryOp op = NULL;     // it's a pointer, so NULL is valid -- but not obvious
```

This is rarely a real problem for function pointers (you can tell from usage),
but it's the reason some teams prefer Style B, where the `*` stays visible. Just
be aware that `BinaryOp` is secretly a pointer, so `NULL`, copying, and "if (op)"
all apply.

---

## 9. Diagram: one alias, many uses

```text
                 typedef int (*BinaryOp)(int, int);
                              |
        +---------------------+---------------------+----------------+
        |                     |                     |                |
        v                     v                     v                v
   a variable           a parameter           a return type     a struct field
   BinaryOp op;     f(BinaryOp cb)         BinaryOp g(void);   { BinaryOp fn; }
        |                     |                     |                |
        +---------------------+---------------------+----------------+
                              |
                  all mean the SAME type; change it in ONE place
```

---

## 10. What to take into section 07

- `typedef` gives a **name** to a function pointer type; define once, use
  everywhere.
- Build it by writing the variable declaration and prefixing `typedef`.
- **Style A** (`typedef int (*BinaryOp)(int,int)`) makes the alias a pointer;
  **Style B** (`typedef int BinaryOpFn(int,int)`) names a function type and you
  add `*` at use. Both are fine.
- Typedefs make complex declarations (like `signal`) readable and are core to
  clean **API design**.
- Remember the alias is secretly a pointer (so `NULL`/guards apply).

With clean syntax in hand, we can finally build the first big *application* of
function pointers: **callbacks** — letting a caller inject behavior into a
function.

➡️ Continue to **`theory/07-callbacks/`**, after running
**`code/06-typedefs/`**.

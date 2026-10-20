# 11 — Dispatch Tables

> A **dispatch table** is an array (or map) of function pointers, indexed by
> something — an opcode, a command name, a menu choice, an event type. Instead of
> a long `switch` that picks behavior, you *look up* the behavior in a table and
> call it. You saw a glimpse in section 05 (array of function pointers) and
> section 09 (the event loop). Now we make it a deliberate technique.

---

## 1. The problem: big `switch` statements

Here's a calculator with a `switch`:

```c
int calculate(char op, int a, int b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        default:  return 0;
    }
}
```

It works, but as cases grow (think: a command interpreter with 50 commands, a
bytecode VM with 200 opcodes), the `switch` becomes huge, and *every* place that
needs to act on the same set of cases repeats a similar `switch`. There's a
cleaner structure.

---

## 2. The idea: index into an array of function pointers

Put each behavior in a function, then store those functions in an **array indexed
by the selector**:

```text
   Index / opcode        Table of function pointers
   --------------        --------------------------
        0        ----->  add
        1        ----->  subtract
        2        ----->  multiply
        3        ----->  divide
```

```c
typedef int (*Op)(int, int);

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b)   { return b ? a / b : 0; }

Op table[] = { add, subtract, multiply, divide };

/* "dispatch": pick by index and call */
int result = table[2](6, 7);   // multiply(6,7) -> 42
```

The `switch` is gone. To act, you **index** the table and **call**. Selecting
behavior became a memory lookup plus an indirect call — both O(1), both trivial.

```text
   table[i](a, b)
        |  index i selects a function pointer
        v
   table[2] = multiply
        |  () calls it
        v
   multiply(6,7) -> 42
```

---

## 3. When the selector isn't a small integer

Array indexing is perfect when your selector is a small, dense integer (0,1,2,3…
— like opcodes or enum values). But often the selector is a **string** (a command
name like `"help"` or `"quit"`) or a sparse integer. Then you use a table of
**{ key, function pointer }** pairs and search/look-up:

```c
typedef void (*CmdFn)(const char *args);

struct command {
    const char *name;
    CmdFn       handler;
    const char *help;
};

struct command commands[] = {
    { "help",  cmd_help,  "show help" },
    { "echo",  cmd_echo,  "print the args" },
    { "quit",  cmd_quit,  "exit the program" },
};

/* dispatch by name: linear search (or a hashmap for many commands) */
CmdFn lookup(const char *name) {
    for (int i = 0; i < N; i++)
        if (strcmp(commands[i].name, name) == 0)
            return commands[i].handler;
    return NULL;   // unknown command
}
```

```text
   "echo"  --search-->  { "echo", cmd_echo, ... }  --call-->  cmd_echo(args)
```

This is exactly how shells, REPLs, debuggers, and chat-bot command systems map a
typed command to a function. With many commands you'd back it by a hashmap (you
already know hashmaps!) — the values stored in the map are **function
pointers**.

> Connection to your background: a dispatch table is often *a hashmap whose
> values are functions.* You've built hashmaps; storing a function pointer as the
> value is the only new idea.

---

## 4. Step-by-step: dispatching a command

Trace `dispatch("echo", "hello")`:

```text
   1. look up "echo" in the table
        -> linear scan: "help"? no. "echo"? YES.
        -> return commands[1].handler  (the address of cmd_echo)

   2. check it's not NULL (known command)

   3. call it: handler("hello")
        -> indirect call into cmd_echo, which prints "hello"
```

Steps 1–3 are: find the function pointer, verify, call. Adding a new command is
just adding one row to the table and writing its handler — **no existing code
changes.** Compare with the `switch`: a new command means editing the switch
*and* possibly every other switch over the same commands.

---

## 5. Dispatch tables vs `switch`: the trade-offs

```text
   SWITCH                                DISPATCH TABLE
   ------                                --------------
   behavior inline in one function       behavior in separate functions
   add case -> edit the switch           add row -> table + new handler
   selector can be anything              array: needs dense int; else use map
   compiler may optimize to a jump       you build the table explicitly
     table anyway (see section 12!)
   all logic in one place (good for      logic distributed (good for many,
     a few cases)                          extensible cases)
   can't change at runtime               table can be MODIFIED at runtime
```

Two points worth emphasizing:

1. **A `switch` over dense integers is often compiled into a jump table anyway**
   (section 12). So for raw speed they're similar. The dispatch *table* is about
   **code organization and runtime flexibility**, not (usually) speed.
2. **A dispatch table can be changed at runtime** — swap a handler, install a new
   command, disable one by setting it to `NULL`. A `switch` is frozen at compile
   time. This is huge for plugins (section 16) and configurable systems.

---

## 6. Designated initializers: clean, safe tables

C99 lets you index the initializer by enum, which makes opcode/event tables
self-documenting and order-independent:

```c
typedef enum { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_COUNT } OpCode;

Op table[OP_COUNT] = {
    [OP_ADD] = add,
    [OP_SUB] = subtract,
    [OP_MUL] = multiply,
    [OP_DIV] = divide,
};
```

Now the table doesn't depend on the order you write the rows, unset slots are
`NULL` (so you can detect "not implemented"), and `OP_COUNT` sizes the array
automatically. This is the professional way to write integer-indexed dispatch
tables — you saw it already in section 09's event loop.

---

## 7. Always validate the selector

The one dangerous bug with array dispatch tables: an **out-of-range index** reads
past the array (undefined behavior, likely a crash or worse). Always bounds-check
before indexing:

```c
int dispatch(OpCode op, int a, int b) {
    if (op < 0 || op >= OP_COUNT || table[op] == NULL)
        return 0;                 // unknown / unimplemented
    return table[op](a, b);
}
```

For string/map tables, the equivalent is "handle the NULL from a failed lookup."
Either way: **never call a slot you didn't verify.** This is the same discipline
as the NULL guard (section 03), applied to table lookups.

---

## 8. Tables of richer entries

Real dispatch tables rarely store a bare function pointer — they store a *struct*
per entry with the pointer plus metadata: a name, help text, expected argument
count, permission flags, a category. The handler is one field among several:

```c
struct command {
    const char *name;
    CmdFn       handler;     // the function pointer
    int         min_args;
    const char *help;
};
```

This is the same shape as a *method table* (section 14's vtable) — a struct
gathering related function pointers and data. Dispatch tables, vtables, and
plugin interfaces are all variations on "a struct/array of function pointers."

---

## 9. Where dispatch tables appear

- **Command interpreters / REPLs / shells:** command name → handler.
- **Bytecode VMs and interpreters:** opcode → operation (section 12).
- **Parsers:** token type → parse routine.
- **Network servers:** request type / message code → handler.
- **Device drivers / OS syscalls:** the Linux kernel's `file_operations` is a
  struct of function pointers; the syscall table is an array of function
  pointers indexed by syscall number.
- **GUI frameworks:** message/event type → handler (section 09's loop).

Once you can spot "an array or map whose elements are function pointers, indexed
by some selector," you'll see dispatch tables everywhere in systems code.

---

## 10. What to take into section 12

- A **dispatch table** maps a selector (int/string/enum) to a function pointer,
  replacing a `switch` with a **lookup + indirect call.**
- Use an **array** for dense integer selectors; a **map/search** for strings or
  sparse keys (often a hashmap of function pointers).
- Use **designated initializers** for clean, order-independent integer tables.
- **Always validate** the selector / handle a NULL lookup before calling.
- The real wins are **organization** and **runtime modifiability**, not raw
  speed (a `switch` may compile to a jump table anyway).

Next, section 12 zooms into the high-performance cousin of dispatch tables —
**jump tables** — and how interpreters use them to execute opcodes fast.

➡️ Continue to **`theory/12-jump-tables/`**, after running
**`code/11-dispatch-tables/`**.

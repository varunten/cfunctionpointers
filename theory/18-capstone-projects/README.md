# 18 — Capstone Projects

> Time to put it all together. These projects are *intentionally* built from the
> patterns of the whole series, so finishing them means you can read and design
> function-pointer APIs with confidence. Each is broken into incremental steps,
> tells you which earlier sections it draws on, and ships with a working
> reference implementation in `code/18-capstone-projects/`.

---

## How to use this section

For each project:

1. **Read the goal and the pattern map** (which sections it uses).
2. **Try the steps yourself first**, in order — they're incremental on purpose
   (store a pointer → call it → put it in a table → build the system).
3. **Then read the reference code** in `code/18-capstone-projects/` and compare.
4. **Extend it** with the "stretch goals" — that's where real understanding sets
   in.

The reference programs are deliberately small but complete and runnable. The
*point* is the architecture, not feature count.

---

## Project 1 — Command Dispatcher (a mini shell)

**Goal:** a REPL-style program where typed commands map to handler functions.

**Patterns:** dispatch table (11), command table with metadata (11),
callbacks (07), API design (15).

**Why it matters:** this is the skeleton of every shell, REPL, debugger console,
and chat-bot command system.

**Incremental steps:**

1. Define `typedef int (*CmdFn)(int argc, char **argv);`.
2. Make a `struct command { const char *name; CmdFn fn; const char *help; };`.
3. Build a table of commands (`help`, `echo`, `add`, `quit`).
4. Write a loop: read a line, split into argv, look up the command, call it.
5. Make `quit` return a sentinel that stops the loop; make `help` print the
   table (so help is data-driven, not hard-coded).

**Stretch goals:** unknown-command suggestions; a command that registers *new*
commands at runtime; per-command argument-count validation.

**Reference:** `code/18-capstone-projects/01_command_dispatcher.c`

```text
   "add 2 3"  -> split -> argv=["add","2","3"]
              -> lookup "add" in table -> cmd_add
              -> cmd_add(3, argv) -> prints 5
```

---

## Project 2 — Calculator via Dispatch Table

**Goal:** an expression evaluator (or operation runner) where operators are
function pointers in a table indexed by the operator.

**Patterns:** dispatch table (11), jump table (12), comparators-style injection
(08).

**Why it matters:** it's a clean, switch-free design and a stepping stone to a
real interpreter.

**Incremental steps:**

1. `typedef double (*Op)(double, double);` and functions `add/sub/mul/div`.
2. Map operator characters to ops (a small lookup, or an array indexed by a
   normalized code).
3. Read `operand operator operand`, dispatch the operator, print the result.
4. Add unary ops (`neg`, `sqrt`) with a second table — show that tables compose.
5. Guard division by zero and unknown operators (validate the selector,
   section 11).

**Stretch goals:** chain operations (a tiny RPN calculator using a stack — which
nudges you toward Project 4); add named functions (`pow`, `max`).

**Reference:** `code/18-capstone-projects/02_calculator.c`

---

## Project 3 — Event System / Observer

**Goal:** a reusable event system: register listeners for named events, emit
events with a payload, and unregister.

**Patterns:** event systems (09), callbacks + user_data (07), register/unregister
API (15).

**Why it matters:** it's the core of GUIs, game engines, and pub/sub systems —
and a genuinely reusable component.

**Incremental steps:**

1. `typedef void (*Listener)(const void *event, void *user_data);`
2. An `EventBus` holding, per event type, a list of `{listener, user_data}`.
3. `bus_on(bus, type, fn, ud)` and `bus_emit(bus, type, event)`.
4. `bus_off` by registration id (robust removal, section 15).
5. Demonstrate multiple listeners reacting to one event, each with its own
   context.

**Stretch goals:** one-shot listeners (auto-unregister after first fire);
priorities (call order); guard against modifying the list during dispatch
(section 09 pitfall).

**Reference:** `code/18-capstone-projects/03_event_system.c`

---

## Project 4 — Stack VM / Calculator Interpreter

**Goal:** a bytecode interpreter with a jump table of opcode handlers (extends
section 12 into a small but real machine).

**Patterns:** jump tables (12), dispatch (11), state (10).

**Why it matters:** this is *the* project that makes interpreters click.
Emulators, language runtimes, and regex engines are all variations.

**Incremental steps:**

1. Define opcodes (`PUSH, ADD, SUB, MUL, DIV, DUP, PRINT, HALT`) and a
   `typedef void (*OpHandler)(VM*);`.
2. A `VM` with bytecode, `pc`, a stack, `sp`, and a `running` flag.
3. The fetch–decode–execute loop: `ops[code[pc++]](vm);`.
4. Implement each handler; handle operands (e.g. `PUSH` reads the next byte).
5. Add an `op_illegal` default in every slot (section 12 safety).

**Stretch goals:** add jumps/branches (`JMP`, `JZ`) — now your VM is
Turing-complete-ish and you're maintaining a program counter like a real CPU; add
a tiny assembler that turns text into bytecode.

**Reference:** `code/18-capstone-projects/04_stack_vm.c`

---

## Project 5 — Plugin System

**Goal:** a host with a plugin contract (struct of function pointers), a
registry, lifecycle (init/run/cleanup), and a version check.

**Patterns:** plugins (16), ops struct / vtable (14–15), registry/list (09/11).

**Why it matters:** it's the capstone of *extensibility* and combines nearly
every pattern.

**Incremental steps:**

1. Define the `Plugin` contract with an `abi_version` field.
2. Write 2–3 plugins (text transforms make good examples).
3. A registry: `register_plugin` (with version check + `init`), `run_all`,
   `shutdown_all`.
4. Make the host drive plugins uniformly through the contract.
5. Show a rejected out-of-version plugin (section 16 safety).

**Stretch goals:** a plugin that itself registers sub-handlers; convert it to
*dynamic* loading with `dlopen`/`LoadLibrary` (see
`code/16-plugin-architectures/dynamic_loading_reference.c`).

**Reference:** `code/18-capstone-projects/05_plugin_system.c`

---

## Project 6 — Mini Game State Machine

**Goal:** a tiny text "game" driven by function-pointer states (menu → playing →
game over), each state handling input and transitioning.

**Patterns:** state machines (10), dispatch (11), polymorphism-ish state objects
(13).

**Why it matters:** game loops and UI flows are state machines; this makes the
pattern concrete and fun.

**Incremental steps:**

1. `typedef void (*StateFn)(Game*, const char *input);` and a `Game` holding the
   current state pointer + game data (score, lives).
2. States: `state_menu`, `state_playing`, `state_gameover`.
3. Each state prints a prompt, interprets input, and may transition
   (`g->current = state_x;`).
4. A loop feeds inputs to `g->current(g, input)` until a terminal state.
5. Carry data (score) across states in the `Game` struct.

**Stretch goals:** a pause state that remembers where to return; an
`on_enter`/`on_exit` hook per state (entry/exit actions, section 10).

**Reference:** `code/18-capstone-projects/06_game_state_machine.c`

---

## Project 7 — Mini Object System / VTable

**Goal:** a small "class system": shapes (circle, rectangle) with a shared vtable
(`area`, `name`, `draw`), used polymorphically.

**Patterns:** vtables (14), polymorphism (13), interfaces (13).

**Why it matters:** it cements that OOP is "structs + shared function-pointer
tables," demystifying C++/Java for good.

**Incremental steps:**

1. A `shape_vtable` struct (`double (*area)(Shape*)`, `const char *(*name)(Shape*)`,
   `void (*draw)(Shape*)`).
2. A `Shape` base with a `vtable` pointer; `Circle`/`Rectangle` embed it
   (section 13's struct-embedding) and add their own fields.
3. One `static const` vtable per shape type; constructors wire them up.
4. Generic code: total area of an array of `Shape*`, printing each via the
   vtable — no `switch`, no type tags.
5. Show two shapes sharing one vtable (the section-14 sharing proof).

**Stretch goals:** add a `Triangle` (note: existing code doesn't change — that's
the win); add a "loud" subclass that overrides `draw` but inherits `area`.

**Reference:** `code/18-capstone-projects/07_object_system.c`

---

## Capstone synthesis: how the patterns relate

By now you should see that the "different" applications are one idea wearing
different hats:

```text
                         a function pointer = an address of code in a variable
                                              |
        +-----------------+-------------------+------------------+----------------+
        |                 |                   |                  |                |
   one pointer        a list of            an array of        a struct of      from a file
   you call           pointers             pointers           pointers         at runtime
   (callback,         (event system,       indexed by a       (vtable,         (plugin via
    comparator)        observer)           selector           ops struct,       dlopen/dlsym)
        07,08           09                  (dispatch/jump      polymorphism)     16
                                             table) 11,12       13,14,15
        |                 |                   |                  |                |
        +-----------------+-------------------+------------------+----------------+
                                              |
                       a state machine is a persistent pointer (10);
                       all of it is "store an address, call through it later"
```

If you can build all seven projects, you have internalized every objective from
`instructions.md`. Function pointers are no longer strange syntax — they're
ordinary pointers that happen to point at executable code, and you can wield them
to build callbacks, dispatchers, interpreters, objects, and plugins.

---

## What's next (beyond the series)

- **Read real headers** with section 17's workflow: `stdlib.h` (`qsort`),
  `pthread.h`, `signal.h`, then a small open-source C library of your choice.
- **Build an emulator** (CHIP-8 is the classic first one) — it's Project 4 grown
  up, and almost pure jump table.
- **Read a driver or `file_operations`** in the Linux kernel — section 17 prepared
  you; it's Project 7 at industrial scale.
- **Design your own library** with a clean callback/ops API (section 15) and have
  someone else use it — the ultimate test of "can I design APIs with function
  pointers."

➡️ Build the projects in **`code/18-capstone-projects/`**. Congratulations on
finishing the series.

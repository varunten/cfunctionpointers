# Function Pointers in C — From First Principles

A deeply illustrated, mental-model-first series that takes you from "what is a
function, really?" to designing your own APIs, interpreters, object systems, and
plugin architectures — all with function pointers.

> **The one idea behind everything:** a function pointer is an *ordinary pointer
> that happens to point at executable code instead of data.* Functions are bytes
> at addresses; an address is an address; storing one in a variable lets you
> choose, at runtime, which code to run. Every section is an elaboration of that.

This series is **not** about memorizing `int (*fp)(int, int)`. It's about
*understanding* what happens in memory and in the CPU, so the syntax becomes
obvious and you can read and design function-pointer code with confidence.

---

## How the series is organized

Two parallel trees with matching section numbers:

- **`theory/NN-topic/README.md`** — the explanation: mental models, memory
  diagrams, execution diagrams, pointer diagrams, step-by-step walkthroughs,
  pitfalls, and "what to take into the next section."
- **`code/NN-topic/`** — small, **incremental**, runnable C programs (plus a
  README per folder) that make each idea concrete. Heavily commented to say *what
  memory changes, what address is stored, and what call occurs.*

**Recommended flow per section:** read the theory README → run the code in order
→ re-read the theory's summary. Sections build on each other; go in order the
first time.

---

## The 18 sections

| #  | Topic | What you'll understand |
|----|-------|------------------------|
| 01 | [Functions in Memory](theory/01-functions-in-memory/README.md) | Functions are machine-instruction bytes in the text segment, at addresses. |
| 02 | [Function Addresses](theory/02-function-addresses/README.md) | A function name *is* its address; `add` and `&add` are the same. |
| 03 | [Basic Function Pointers](theory/03-basic-function-pointers/README.md) | A variable that holds a function's address. |
| 04 | [Calling Through Pointers](theory/04-calling-through-pointers/README.md) | Direct vs indirect calls; what the CPU actually does. |
| 05 | [Function Pointer Syntax](theory/05-function-pointer-syntax/README.md) | Decode any declaration with the Right-Left rule. |
| 06 | [Typedefs](theory/06-typedefs/README.md) | Make function-pointer types readable. |
| 07 | [Callbacks](theory/07-callbacks/README.md) | Inversion of control; `void *user_data`. |
| 08 | [Comparators and Sorting](theory/08-comparators-and-sorting/README.md) | Why `qsort` needs a callback; injecting behavior. |
| 09 | [Event Systems](theory/09-event-systems/README.md) | Listeners, dispatch, the event loop. |
| 10 | [State Machines](theory/10-state-machines/README.md) | A state *is* a function pointer; transitions are reassignments. |
| 11 | [Dispatch Tables](theory/11-dispatch-tables/README.md) | Replace big `switch`es with tables of function pointers. |
| 12 | [Jump Tables](theory/12-jump-tables/README.md) | How interpreters/VMs execute opcodes fast. |
| 13 | [Polymorphism in C](theory/13-polymorphism-in-c/README.md) | Common interface, different implementations, `self`. |
| 14 | [VTables in C](theory/14-vtables-in-c/README.md) | Virtual dispatch — exactly how C++ works underneath. |
| 15 | [API Design](theory/15-api-design/README.md) | Exposing callbacks; ownership and lifetime. |
| 16 | [Plugin Architectures](theory/16-plugin-architectures/README.md) | Function-pointer contracts; `dlopen`/`dlsym`. |
| 17 | [Reading Real Code](theory/17-reading-real-code/README.md) | Recognize the patterns in `qsort`, pthreads, the kernel. |
| 18 | [Capstone Projects](theory/18-capstone-projects/README.md) | Build 7 complete programs using everything. |

---

## Building and running the code

All examples are portable C11 and were compiled clean with
`gcc -Wall -Wextra -std=c11`. From any code folder:

```sh
gcc 01_example.c -o example && ./example      # Linux/macOS
gcc 01_example.c -o example.exe ; .\example.exe   # Windows / MinGW
```

A few programs link the math library (`-lm`) or read from `stdin`; their folder
READMEs note this. The dynamic-plugin example
(`code/16-plugin-architectures/dynamic_loading_reference.c`) is OS-specific
reference documentation and is intentionally not compiled by the default
commands.

### Build everything at once

```sh
# Linux/macOS
find code -name '*.c' ! -name 'dynamic_loading_reference.c' \
  -exec gcc -Wall -Wextra -std=c11 {} -o /tmp/a.out -lm \;
```

```powershell
# Windows PowerShell
Get-ChildItem code -Recurse -Filter *.c |
  Where-Object Name -ne 'dynamic_loading_reference.c' |
  ForEach-Object { gcc -Wall -Wextra -std=c11 $_.FullName -o a.exe -lm }
```

---

## The big picture (one diagram for the whole series)

```text
          a function pointer = the address of code, held in a variable
                                       |
   +----------+----------+-------------+-------------+-----------+-----------+
   |          |          |             |             |           |           |
  store     call       pass to       put many      put in a    bundle in   load from
  an addr   through    a function    in a list     table       a struct    a file at
  (03)      it (04)    (callback,    (events,      (dispatch/  (vtable,    runtime
                        07,08)        observer 09)  jump 11,12) plugins     (plugin
                                                                14,15,16)    16)
                                       |
                a persistent pointer that evolves = a state machine (10)

   Reading real code (17) = recognizing which of these you're looking at.
   Capstones (18) = building all of them.
```

---

## What you'll be able to do at the end

- Explain where functions live in memory and what a function address is.
- Read and write any function-pointer declaration without fear.
- Use and design callbacks (with proper `user_data` and lifetime handling).
- Build dispatch tables, jump-table interpreters, and state machines.
- Implement polymorphism and vtables in C — and understand how C++ does it.
- Architect plugin systems around function-pointer contracts.
- Read function-pointer-heavy real-world code (stdlib, pthreads, the kernel).

Function pointers will no longer look like strange syntax. They'll look like what
they are: **ordinary pointers that happen to point at executable code.**

Start with **[theory/01-functions-in-memory/README.md](theory/01-functions-in-memory/README.md)**.

# Code — 01 Functions in Memory

These tiny programs make the ideas from `theory/01-functions-in-memory/`
concrete. Run them in order.

## Files

| File | What it shows |
|------|----------------|
| `01_hello_function.c` | A function is something the CPU *executes*; `main` calls `add`. |
| `02_where_do_things_live.c` | Prints the addresses of code, globals, heap, and stack so you can *see* the segments. |
| `03_one_code_many_calls.c` | One function's code is shared; recursion shows many live frames of one code. |

## Build & run (any platform)

```sh
gcc 01_hello_function.c -o hello && ./hello
gcc 02_where_do_things_live.c -o layout && ./layout
gcc 03_one_code_many_calls.c -o frames && ./frames
```

On Windows with MinGW it's the same, just `hello.exe`:

```sh
gcc 02_where_do_things_live.c -o layout.exe
layout.exe
```

## What to look for

- In `02`, notice the **code addresses are low** and **stack addresses are
  high** — exactly the layout diagram from the theory file. The relative order
  is the lesson; exact numbers vary per OS/run (and ASLR shuffles them between
  runs).
- In `03`, every recursive call prints a *different* address for its local
  variable (different stack frames) but the function's *code* address never
  changes.

> These programs only *read* and *print* addresses. We don't store a function
> address in a pointer yet — that's section 03. First, just believe that
> functions and data both have addresses.

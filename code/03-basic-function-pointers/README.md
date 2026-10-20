# Code — 03 Basic Function Pointers

Declare a function pointer, store a function's address in it, prove the stored
value equals the function's address, and swap which function it points to.

## Files

| File | What it shows |
|------|----------------|
| `01_declare_and_store.c` | Declare `int (*fp)(int,int)`, assign `add`, print that `fp == add`. |
| `02_swap_target.c` | One pointer, reassigned to different functions; the stored address changes. |
| `03_null_pointer.c` | A `NULL` function pointer and the guard-before-call pattern. |

> We *store* and *inspect* addresses here. We don't *call* through the pointer
> until section 04 (one file previews it so the idea isn't left dangling).

## Build & run

```sh
gcc 01_declare_and_store.c -o store && ./store
gcc 02_swap_target.c       -o swap  && ./swap
gcc 03_null_pointer.c      -o nullp && ./nullp
```

## What to look for

- In `01`, `fp` and `add` print the **same** address — `fp` literally holds
  add's address.
- In `02`, watch the stored address change as you reassign `fp`. Same variable,
  different target.
- In `03`, the guarded call is skipped when `fp` is `NULL`, then runs once `fp`
  points somewhere — the exact pattern real callback APIs use.

# Code — 07 Callbacks

Build callbacks from the ground up: inject behavior into a loop, carry state
with `void *user_data`, make callbacks optional, and let a callback stop the
host loop.

## Files

| File | What it shows |
|------|----------------|
| `01_for_each.c` | The core idea: `for_each` calls your function on each element. Same loop, different behavior. |
| `02_user_data.c` | `void *user_data` carries per-call state (summing into your own variable). |
| `03_optional_callback.c` | Optional progress callback with the `NULL` guard. |
| `04_stoppable.c` | Callback returns nonzero to stop the host loop early (search pattern). |

## Build & run

```sh
gcc 01_for_each.c          -o foreach  && ./foreach
gcc 02_user_data.c         -o udata    && ./udata
gcc 03_optional_callback.c -o optcb    && ./optcb
gcc 04_stoppable.c         -o stoppable && ./stoppable
```

## What to look for

- In `02`, the library function `for_each` never knows it's helping you sum —
  it just hands your `user_data` back. The casting from `void *` to `int *`
  happens inside *your* callback.
- In `04`, the loop stops as soon as your callback says so. The host controls
  iteration; your policy controls when to quit. This is exactly how "find first
  match" APIs are built.

# Code — 15 API Design with Function Pointers

The standard patterns for exposing function pointers from a library, plus the
lifetime issue that makes or breaks such APIs.

## Files

| File | What it shows |
|------|----------------|
| `01_logger_api.c` | Pattern A: a single settable logger callback with `user_data` and a NULL default. |
| `02_ops_struct.c` | Pattern B: a struct of callbacks (alloc/free/on_error) the user fills in. |
| `03_register_api.c` | Pattern C: register/unregister handlers, returning an id for removal. |
| `04_lifetime_demo.c` | The dangling-`user_data` bug and the caller-owns fix — the core lifetime lesson. |

## Build & run

```sh
gcc 01_logger_api.c    -o logapi && ./logapi
gcc 02_ops_struct.c    -o opsapi && ./opsapi
gcc 03_register_api.c  -o regapi && ./regapi
gcc 04_lifetime_demo.c -o lifeapi && ./lifeapi
```

## What to look for

- `02` is shaped like SQLite/Lua allocator hooks and the kernel's
  `file_operations`: bundle related callbacks in a struct.
- `04` is the most important: it shows *correct* lifetime management (the data
  outlives the registration). Read the comments on what would go wrong if it
  didn't — that's the #1 callback-API bug in the wild.

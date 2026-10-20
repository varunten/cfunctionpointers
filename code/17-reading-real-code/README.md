# Code — 17 Reading Real Code

Practice *recognizing* the patterns in realistic-looking code. Each file is a
small, self-contained imitation of a real API shape, annotated so you learn to
read the real thing.

## Files

| File | Imitates | Pattern to recognize |
|------|----------|----------------------|
| `01_like_qsort.c` | `qsort`/`bsearch` | callback comparator `(const void*,const void*)->int` |
| `02_like_pthread.c` | `pthread_create` | async callback `void*(*)(void*)` + `void *arg` |
| `03_like_file_ops.c` | Linux `file_operations` | struct of function pointers (vtable/ops), `self` first param, NULL = optional |
| `04_decode_declarations.c` | scary headers | decoding declarations with the Right-Left rule + mental typedef |

## Build & run

```sh
gcc 01_like_qsort.c          -o rqsort  && ./rqsort
gcc 02_like_pthread.c        -o rthread && ./rthread
gcc 03_like_file_ops.c       -o rfops   && ./rfops
gcc 04_decode_declarations.c -o rdecode && ./rdecode
```

## What to look for

- These deliberately use the *exact shapes* of real APIs (minus the OS bits) so
  that when you open the real `pthread.h` or kernel `fs.h`, you think "I've seen
  this — it's an async callback / an ops struct."
- `04` walks several real-world declarations and shows the mental-typedef trick
  that makes them readable.

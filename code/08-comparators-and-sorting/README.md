# Code — 08 Comparators and Sorting

Use the standard library's `qsort` with comparison callbacks. Same `qsort`,
different orderings — chosen entirely by which comparator you pass.

## Files

| File | What it shows |
|------|----------------|
| `01_qsort_int.c` | Sort an `int` array ascending and descending by swapping the comparator. |
| `02_qsort_structs.c` | Sort an array of structs by different fields (age, then name). |
| `03_trace_compare.c` | A comparator that prints each call, so you can *watch* `qsort` calling back into your code. |
| `04_bsearch.c` | Reuse the same comparator with `bsearch` to find an element. |

## Build & run

```sh
gcc 01_qsort_int.c     -o qint    && ./qint
gcc 02_qsort_structs.c -o qstruct && ./qstruct
gcc 03_trace_compare.c -o qtrace  && ./qtrace
gcc 04_bsearch.c       -o bsrch   && ./bsrch
```

## What to look for

- In `03`, the printed comparison calls are `qsort` reaching back into *your*
  function repeatedly — the indirect calls from section 04, happening inside the
  library. The exact pairs/order depend on your libc's sort implementation.
- In `02`, the data never changes — only the comparator does — yet the output
  order changes completely. That's behavior injection.

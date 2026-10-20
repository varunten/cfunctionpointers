# Code — 04 Calling Through Pointers

Actually run functions through a pointer, see both call syntaxes are equal, and
watch one call site produce different results as the stored address changes.

## Files

| File | What it shows |
|------|----------------|
| `01_two_call_styles.c` | `fp(2,3)` and `(*fp)(2,3)` give identical results. |
| `02_runtime_choice.c` | One `fp(a,b)` call site runs add or multiply based on user input — the point of indirect calls. |
| `03_direct_vs_indirect.c` | A direct call and an indirect call side by side, with comments on what the CPU does differently. |

## Build & run

```sh
gcc 01_two_call_styles.c    -o styles && ./styles
gcc 02_runtime_choice.c     -o choice && ./choice
gcc 03_direct_vs_indirect.c -o di     && ./di
```

To *see* the direct-vs-indirect difference in real machine code (optional, very
illuminating):

```sh
gcc -O1 -S 03_direct_vs_indirect.c -o di.s
```

Then open `di.s` and find the `call` instructions. The direct call targets a
named symbol; the indirect call goes through a register (e.g. `call *%rax` /
`call rax`). That is the entire mechanical difference from the theory file.

## What to look for

- `02` is the payoff: the program never changes *which line* it calls, only
  *what `fp` points to*, and the behavior changes accordingly.

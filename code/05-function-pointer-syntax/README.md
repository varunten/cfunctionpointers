# Code — 05 Function Pointer Syntax

Drill the declaration syntax until it's boring. Read each declaration with the
Right-Left rule, then run the program to confirm your reading.

## Files

| File | What it shows |
|------|----------------|
| `01_decl_gallery.c` | A gallery of function-pointer declarations, each with a comment giving the Right-Left reading, all used so they actually compile and run. |
| `02_parens_matter.c` | The crucial contrast: `int (*fp)(int,int)` vs `int *fp(int,int)`. |
| `03_array_of_fps.c` | An array of function pointers, indexed and called (preview of dispatch tables). |

## Build & run

```sh
gcc 01_decl_gallery.c  -o gallery && ./gallery
gcc 02_parens_matter.c -o parens  && ./parens
gcc 03_array_of_fps.c  -o arr     && ./arr
```

## What to look for

- In `02`, note one form is a *pointer variable* you can assign a function to,
  and the other is a *function declaration* — the parentheses are the only
  difference and they change everything.
- In `03`, `ops[i](a,b)` selects and calls a function by index. That's the seed
  of section 11 (dispatch tables).

# Code — 06 Typedefs

See how `typedef` turns ugly, repeated function-pointer types into one clean
name used everywhere.

## Files

| File | What it shows |
|------|----------------|
| `01_before_after.c` | The same program written with raw types vs a `typedef`. |
| `02_typedef_everywhere.c` | One typedef used as a variable, parameter, return type, and struct field. |
| `03_two_styles.c` | Style A (pointer-in-typedef) vs Style B (function-type typedef). |

## Build & run

```sh
gcc 01_before_after.c       -o ba    && ./ba
gcc 02_typedef_everywhere.c -o tev   && ./tev
gcc 03_two_styles.c         -o styles2 && ./styles2
```

## What to look for

- In `02`, notice the typedef name reads the same in every position — that
  consistency is the whole point.
- In `03`, both styles compile and call identically; the only difference is
  whether you write the `*` in the typedef or at the use site.

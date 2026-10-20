# Code — 14 VTables in C

Refactor section 13's per-object method pointers into a shared vtable per type —
exactly how C++ virtual functions work under the hood.

## Files

| File | What it shows |
|------|----------------|
| `01_vtable_basics.c` | Object holds one vtable pointer; all objects of a type share one `static const` vtable. Virtual dispatch via `a->vtable->speak(a)`. |
| `02_share_proof.c` | Many objects of the same type literally share the same vtable address. |
| `03_override_inherit.c` | A "derived" vtable that overrides one method and reuses (inherits) another. |

## Build & run

```sh
gcc 01_vtable_basics.c    -o vt     && ./vt
gcc 02_share_proof.c      -o vshare && ./vshare
gcc 03_override_inherit.c -o vover  && ./vover
```

## What to look for

- In `02`, every dog prints the *same* vtable address — proof the table is
  shared, not copied per object. That's the memory win over section 13.
- In `03`, the derived vtable puts a new function pointer in the `speak` slot
  (override) but copies the base's pointer in the `describe` slot (inherit).
  Overriding and inheriting are just "which pointer goes in which slot."

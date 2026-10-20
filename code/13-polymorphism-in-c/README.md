# Code — 13 Polymorphism in C

Give different "types" a common interface with different implementations, using
function pointers inside structs.

## Files

| File | What it shows |
|------|----------------|
| `01_animal_speak.c` | The core trick: each `animal` carries its own `speak` function pointer; one loop, different behavior. |
| `02_self_param.c` | Why the `self` parameter exists — methods read their object's data through it. |
| `03_interface.c` | A multi-method interface (`speak`, `move`, `legs`); code written against the interface works for any animal. |

## Build & run

```sh
gcc 01_animal_speak.c -o animals && ./animals
gcc 02_self_param.c   -o selfp   && ./selfp
gcc 03_interface.c    -o iface   && ./iface
```

## What to look for

- In `01`/`03`, the loop that makes every animal speak/move has **no switch and
  no type tag** — adding a new animal never touches it. That's the payoff of
  polymorphism.
- In `02`, drop the `self` argument in your head and notice the method would have
  no way to know *which* animal's name to print. Every C "method" needs `self`.

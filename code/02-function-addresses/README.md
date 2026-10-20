# Code — 02 Function Addresses

Make function addresses real: print them, prove `add == &add`, and see that
different functions sit at different addresses.

## Files

| File | What it shows |
|------|----------------|
| `01_print_addresses.c` | Print the addresses of several functions with `%p`. Different funcs → different addresses. |
| `02_name_equals_amp.c` | `add`, `&add`, and `*add` all denote the same address. |
| `03_name_vs_call.c` | The difference between *naming* a function (its address) and *calling* it (`()`), made explicit. |

## Build & run

```sh
gcc 01_print_addresses.c -o addrs && ./addrs
gcc 02_name_equals_amp.c  -o eq    && ./eq
gcc 03_name_vs_call.c     -o nc    && ./nc
```

## What to look for

- Run `01` twice. The numbers may differ between runs (ASLR) but the
  *relationships* are stable: each function has a distinct address.
- In `02`, every printed address on the comparison lines is **identical** — that
  is the proof that `add`, `&add`, and even `*add` all mean "the address of
  add's code."
- `03` is the conceptual punchline: no parentheses = a value (the address);
  parentheses = an action (the call).

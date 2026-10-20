# Code — 11 Dispatch Tables

Replace `switch` statements with arrays/maps of function pointers indexed by a
selector.

## Files

| File | What it shows |
|------|----------------|
| `01_switch_baseline.c` | A calculator with a `switch` — the thing we're replacing. |
| `02_array_dispatch.c` | The same calculator as an enum-indexed array of function pointers (with bounds checking). |
| `03_command_table.c` | A string-keyed command table ({name, handler, help}) — a mini REPL dispatcher. |
| `04_runtime_modify.c` | Swap a handler in the table at runtime — something a `switch` cannot do. |

## Build & run

```sh
gcc 01_switch_baseline.c -o cswitch && ./cswitch
gcc 02_array_dispatch.c  -o carray  && ./carray
gcc 03_command_table.c   -o cmds    && ./cmds
gcc 04_runtime_modify.c  -o cmod    && ./cmod
```

## What to look for

- `02` uses designated initializers (`[OP_ADD] = add`) and validates the index
  before calling — the safe, professional pattern.
- `03` is how shells/REPLs work: type a command, look it up, call its handler.
- `04` is the superpower a `switch` lacks: the table is data, so you can change
  behavior while the program runs.

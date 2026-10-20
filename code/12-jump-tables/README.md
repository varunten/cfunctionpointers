# Code — 12 Jump Tables

Build a real (tiny) bytecode interpreter driven by a jump table of function
pointers — the fetch/decode/execute loop at the heart of every VM.

## Files

| File | What it shows |
|------|----------------|
| `01_opcode_table.c` | The minimal idea: an `ops[]` table indexed by opcode, called in a loop. |
| `02_stack_vm.c` | A complete tiny stack VM (PUSH/ADD/SUB/MUL/PRINT/HALT) with a program counter and operands. |
| `03_illegal_handler.c` | Fill every slot with a default `op_illegal` so unknown opcodes are handled, not crashes. |

## Build & run

```sh
gcc 01_opcode_table.c    -o ops    && ./ops
gcc 02_stack_vm.c        -o vm     && ./vm
gcc 03_illegal_handler.c -o illegal && ./illegal
```

## What to look for

- In `02`, the dispatch loop (`ops[code[pc++]](vm)`) never changes no matter how
  many opcodes exist. Each opcode is just a small function plus a table row —
  exactly how CPython/Lua/JVM are structured.
- In `03`, an out-of-set opcode runs `op_illegal` instead of dereferencing NULL.
  Pre-filling the table is the safe way to build a jump table.

# 12 — Jump Tables

> A **jump table** is the performance-focused, lower-level cousin of the dispatch
> table. The term gets used two ways: (1) the *compiler-generated* table a
> `switch` becomes, and (2) the *interpreter pattern* where an array of function
> pointers (or label addresses) drives execution of opcodes/commands. Both are
> the same core idea — "index into a table of addresses and jump" — applied to
> making interpreters and VMs fast.

---

## 1. Two meanings of "jump table"

**Meaning A — what a `switch` compiles into.** When you write a `switch` over
dense integer cases, the compiler often doesn't generate a chain of comparisons.
Instead it builds an internal array of *code addresses* (one per case) and emits:
"index the array by the value, jump there." That hidden array is a jump table.

**Meaning B — the interpreter pattern.** When *you* explicitly build an array of
function pointers indexed by an opcode and call through it in a loop, that's a
jump table you wrote by hand — the standard structure of a bytecode interpreter.

This section connects both: a `switch` *gives you* meaning A automatically; the
function-pointer version *is* meaning B and gives you flexibility a `switch`
can't. Understanding the relationship demystifies how interpreters work.

---

## 2. How a `switch` becomes a jump table (meaning A)

Consider:

```c
switch (opcode) {
    case 0: return do_add(a, b);
    case 1: return do_sub(a, b);
    case 2: return do_mul(a, b);
    case 3: return do_div(a, b);
}
```

A naive compiler could test `opcode == 0`, then `== 1`, etc. — O(n) comparisons.
But for dense cases (0,1,2,3), a smart compiler instead emits something like:

```text
   jump_table:
       +-----------------------+
       | addr of case 0 code   |   <- jump_table[0]
       | addr of case 1 code   |   <- jump_table[1]
       | addr of case 2 code   |   <- jump_table[2]
       | addr of case 3 code   |   <- jump_table[3]
       +-----------------------+

   to dispatch:
       if opcode in [0,3]:
           jump to jump_table[opcode]     ; ONE indexed jump, O(1)
```

So the `switch` is *already* "index a table of addresses and jump." The
addresses here are **code labels**, not function pointers, but the mechanism is
identical to an indirect call: take an address from a table, set the instruction
pointer to it.

> This is why section 11 said a `switch` and a dispatch table often have similar
> speed — the compiler turns a dense `switch` into the same kind of indexed jump
> you'd build by hand.

---

## 3. The interpreter pattern (meaning B)

Now *you* build it explicitly. An interpreter/VM executes a stream of
**opcodes** (small integers, each meaning an operation). The classic structure:

```c
typedef void (*OpHandler)(VM *vm);

OpHandler ops[OP_COUNT] = {
    [OP_PUSH] = op_push,
    [OP_ADD]  = op_add,
    [OP_PRINT]= op_print,
    [OP_HALT] = op_halt,
};

/* the fetch-decode-execute loop */
while (vm->running) {
    uint8_t opcode = vm->code[vm->pc++];   // FETCH the next opcode
    ops[opcode](vm);                       // DECODE (index) + EXECUTE (call)
}
```

```text
   bytecode:   [ OP_PUSH 5 ][ OP_PUSH 3 ][ OP_ADD ][ OP_PRINT ][ OP_HALT ]
                    |
                    | fetch opcode, index ops[], call handler
                    v
   ops[OP_PUSH] -> op_push   (push 5, then 3)
   ops[OP_ADD]  -> op_add    (pop 3 and 5, push 8)
   ops[OP_PRINT]-> op_print  (print 8)
   ops[OP_HALT] -> op_halt   (stop the loop)
```

This **fetch → decode → execute** loop is the beating heart of CPython,
the JVM, Lua, the Ruby VM, WebAssembly runtimes, and basically every bytecode
interpreter. The "decode" step is an array index; the "execute" step is an
indirect call through a function pointer. *That's* a jump table interpreter.

---

## 4. Step-by-step: executing tiny bytecode

Let's run `PUSH 5, PUSH 3, ADD, PRINT, HALT` on a stack VM:

```text
   pc=0: fetch OP_PUSH; op_push reads operand 5, pushes it.   stack: [5]      pc->2
   pc=2: fetch OP_PUSH; op_push reads operand 3, pushes it.   stack: [5,3]    pc->4
   pc=4: fetch OP_ADD;  op_add pops 3 and 5, pushes 8.        stack: [8]      pc->5
   pc=5: fetch OP_PRINT;op_print pops 8, prints "8".          stack: []       pc->6
   pc=6: fetch OP_HALT; op_halt sets running=0.               loop ends.
```

Every line is: read an opcode, use it to index `ops[]`, call the handler. The
program counter `pc` walks the bytecode; the jump table turns each opcode into
the right function call. Notice the resemblance to a state machine (section 10) —
an interpreter is a state machine driven by an instruction stream.

---

## 5. Why use function pointers instead of a `switch` here?

For an interpreter, the function-pointer jump table buys you things a `switch`
struggles with:

- **Uniform structure:** every opcode is a row in a table and a small function.
  Adding an opcode = add a function + a table row. The dispatch loop never grows.
- **Runtime configurability:** you can swap handlers — e.g. install a *tracing*
  or *debugging* version of every op, or patch one opcode, without recompiling
  the loop (section 11's runtime-modify idea).
- **Separation:** each operation is independently testable and readable.

A `switch` can be just as fast (it compiles to meaning-A jump table), but it's
one giant function and is frozen at compile time. For a real VM, the
function-pointer table (or the even-faster "computed goto" variant below) is the
common professional choice.

```text
   SWITCH interpreter                    JUMP-TABLE interpreter
   -----------------                     ----------------------
   one huge switch in the loop           one small function per opcode
   compiler builds the jump table        you build the table (it's data)
   frozen at compile time                swappable at runtime
   fast                                  fast (sometimes faster: see goto)
```

---

## 6. Bonus: computed goto (the fastest dispatch)

GCC and Clang offer a non-standard extension, **labels as values** / **computed
goto**, that takes the address of a *label* (`&&label`) and jumps to it via a
table. This is meaning-A done by hand and is the fastest known portable-ish
interpreter dispatch (it gives the CPU's branch predictor better information):

```c
static void *table[] = { &&do_push, &&do_add, &&do_print, &&do_halt };
goto *table[code[pc++]];      // jump directly to the label

do_push:  /* ... */  goto *table[code[pc++]];
do_add:   /* ... */  goto *table[code[pc++]];
/* ... */
```

You don't need to use this — it's compiler-specific and harder to read — but it's
worth *recognizing*, because high-performance interpreters (CPython's main eval
loop, for instance) use exactly this trick. Note it stores **label addresses**,
the closest cousin to a function pointer: an address you jump to. It cements the
unifying idea: *dispatch is always "index a table of addresses, then jump."*

---

## 7. Operands, the program counter, and a real VM

A complete VM has a bit more machinery, all orthogonal to the dispatch:

- A **program counter (`pc`)** indexing into the bytecode array.
- **Operands** following some opcodes (e.g. `PUSH 5` — the `5` is read from the
  bytecode after the opcode, advancing `pc`).
- A **stack** (or registers) the handlers operate on.
- A **`running` flag** that `HALT` clears to end the loop.

The function-pointer jump table is the *dispatch* part; these are the *state* the
handlers manipulate. In `code/12-jump-tables/` we build a tiny but complete stack
VM so you can see all the pieces cooperate.

---

## 8. Where jump tables show up (real systems)

- **Language VMs:** CPython, JVM, Lua, Ruby (YARV), WebAssembly engines — opcode
  → handler.
- **Regex engines** and **parsers:** token/state → action.
- **CPU emulators** (NES, GameBoy, CHIP-8 emulators): each machine opcode →
  a function emulating it. A great beginner project, and pure jump table.
- **Protocol decoders:** message type byte → decoder function.
- **The OS syscall table:** syscall number → kernel handler (an array of
  function pointers indexed by an integer — a jump table by any other name).

If you ever build an emulator or a little language, the jump table *is* the
project's core. Recognizing the pattern now means those projects won't mystify
you.

---

## 9. Pitfalls (mostly shared with dispatch tables)

| Pitfall | Consequence | Fix |
|---|---|---|
| Opcode out of range | Indexes past the table → UB/crash | Validate `opcode < OP_COUNT` before indexing |
| Unimplemented opcode (NULL slot) | Crash on call | Initialize unused slots to a `op_illegal` handler, not NULL |
| `pc` runs past the bytecode | Reads garbage | Ensure programs end with `HALT`; bounds-check `pc` |
| Forgetting to advance `pc` for operands | Misreads the stream | Each handler must advance `pc` correctly |

A nice trick: fill *every* table slot with a default `op_illegal` handler first,
then overwrite the real ones. Then an unknown opcode calls `op_illegal` (which
can report an error) instead of dereferencing NULL.

---

## 10. What to take into section 13

- A **jump table** = "index a table of addresses and jump." It's what a dense
  `switch` *compiles to* (code labels) and what an interpreter *builds by hand*
  (function pointers).
- The **fetch–decode–execute** loop over an `ops[]` table is the heart of every
  bytecode VM.
- Function-pointer tables give **uniform structure** and **runtime
  configurability**; `switch`/computed-goto can be marginally faster.
- Always **validate the opcode**; prefer a default `op_illegal` over NULL slots.

We've now thoroughly covered function pointers as *selectors of behavior*. The
final arc (13–16) uses them to build **objects**: bundling data with the
functions that act on it. First, polymorphism in C.

➡️ Continue to **`theory/13-polymorphism-in-c/`**, after running
**`code/12-jump-tables/`**.

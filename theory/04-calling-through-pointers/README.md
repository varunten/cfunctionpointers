# 04 — Calling Through Pointers

> You can store a function's address in a pointer. Now: **how do you run the
> function the pointer points at?** And — the part that demystifies everything —
> **what does the CPU actually do** when you call through a pointer versus
> calling normally? Spoiler: it's almost the same operation.

---

## 1. Calling through the pointer

Given:

```c
int add(int a, int b) { return a + b; }
int (*fp)(int, int) = add;
```

you call the function it points to like this:

```c
int r1 = (*fp)(2, 3);   // "classic" style: dereference, then call
int r2 = fp(2, 3);      // "modern" style: just call -- equally valid
```

**Both do exactly the same thing.** Modern C lets you call a function pointer as
if it were a function name, so `fp(2,3)` works. The older `(*fp)(2,3)` spells out
"dereference `fp` to get the function, then call it." Use whichever you like;
`fp(2,3)` is more common today, but `(*fp)(2,3)` makes the indirection visible
and matches the declaration syntax nicely.

Why are both legal? Because of the same decay rule from section 02: a function
*name* decays to a pointer, and a function *pointer* is already that pointer. The
call operator `()` works on "a thing that is (or decays to) a function pointer."
Both `add` and `fp` qualify, which is why `add(2,3)` and `fp(2,3)` look
identical.

```text
   add(2,3)     -> call the function whose address is baked in (add)
   fp(2,3)      -> call the function whose address is currently in fp
   (*fp)(2,3)   -> same as fp(2,3), written the explicit way
```

---

## 2. Direct call vs indirect call — the core distinction

This is the heart of the section.

### Direct call

```c
add(2, 3);
```

When the compiler sees `add(2,3)`, it knows *at compile time* exactly which
function you mean. It bakes the target address right into the instruction. The
generated machine code is essentially:

```text
   call 0x401000        ; the address is a CONSTANT inside the instruction
```

The CPU doesn't have to look anything up. The destination is part of the
instruction itself. This is a **direct call**.

### Indirect call

```c
fp(2, 3);
```

Now the compiler does **not** know which function will run — `fp` could hold
`add`'s address, or `multiply`'s, or anything, decided at runtime. So it
generates code that says "call whatever address is *in this variable*":

```text
   mov  rax, [fp]       ; load the address stored in fp into a register
   call rax             ; call the address held in the register
```

This is an **indirect call**: the destination comes from a register/memory, not
from a constant in the instruction.

```text
   DIRECT CALL                          INDIRECT CALL
   -----------                          -------------
   add(2,3);                            fp(2,3);

   call 0x401000                        mov  rax, [fp]
        ^                               call rax
        |                                    ^
   address is a constant                address comes from a variable
   known at compile time                decided at run time
```

> **That single difference — "address baked in" vs "address fetched from a
> variable" — is the entire mechanical essence of function pointers.** Same
> `call` machinery, same jump, same stack frame setup. The only change is
> *where the destination address came from.*

---

## 3. Step-by-step: what the CPU does on `fp(2, 3)`

Let's trace it with `fp` currently holding `add`'s address (`0x401000`).

```text
   STATE before the call:
     fp = 0x401000      (points at add's code)
     RIP = 0x402100     (somewhere in main, at the fp(2,3) line)

   STEP 1: evaluate arguments
     put 2 and 3 where the calling convention says (e.g. registers edi, esi)

   STEP 2: fetch the target address from fp
     read the 8 bytes of fp -> 0x401000

   STEP 3: push the return address
     remember where to come back to (the instruction after the call),
     say 0x402108, by pushing it onto the stack

   STEP 4: jump
     set RIP = 0x401000    <-- now executing add's first instruction

   STEP 5: add runs
     it computes 2 + 3 = 5, puts 5 in the return register (eax)

   STEP 6: add returns
     it pops the saved return address (0x402108) and sets RIP back to it

   STEP 7: back in main
     the result 5 is read from the return register and stored in r1
```

Compare to a direct `add(2,3)`: **only STEP 2 disappears** (the address is
already in the instruction). Everything else — arguments, return address, jump,
run, return — is identical. An indirect call is a direct call plus "first, go
read the address out of a variable."

---

## 4. The execution diagram (the one from the brief)

```text
   fp(2, 3)
      |
      |  STEP 2: read the address stored in fp
      v
   0x401000                 (the value fp holds)
      |
      |  STEP 4: CPU sets its instruction pointer here (a JUMP)
      v
   CPU begins fetching/executing instructions at 0x401000
      |
      v
   add() runs: computes 2 + 3 = 5
      |
      |  STEP 6: ret -> jump back to saved return address
      v
   control returns to main, with 5 as the result
```

Read it top to bottom. Pointer → address → CPU jump → function executes →
return. That five-line chain *is* "calling through a function pointer."

---

## 5. Why indirect calls are slightly slower (and why we don't care)

Because of STEP 2, an indirect call has to fetch the address before jumping, and
the CPU's branch predictor has a harder time guessing where it's going (the
target can change). So indirect calls can be a hair slower than direct calls.

In practice this almost never matters:

- The cost is tiny (often a single-digit number of CPU cycles when predicted).
- The flexibility you gain — choosing behavior at runtime — is worth enormously
  more than those cycles in the vast majority of code.
- Modern CPUs have *indirect branch predictors* that learn common targets.

So: know that the difference exists (it's why hot inner loops sometimes avoid
function pointers), but don't let it scare you away from using them.

---

## 6. Arguments and return values still work normally

A function pointer's *type* tells the compiler the parameter and return types, so
the **calling convention** (how arguments are passed and results returned) is set
up correctly. That's why the type has to match (section 03):

```c
int (*fp)(int, int) = add;
int r = fp(2, 3);     // compiler knows: pass two ints, expect an int back
```

If you lied about the type — say you called through a pointer whose type didn't
match the real function — arguments would be placed wrong and you'd get garbage
or a crash. The type is not decoration; it's the contract that makes the call
mechanically correct.

---

## 7. A complete minimal example to anchor everything

```c
#include <stdio.h>

int add(int a, int b)      { return a + b; }
int multiply(int a, int b) { return a * b; }

int main(void) {
    int (*fp)(int, int);   // a box for a function address

    fp = add;
    printf("%d\n", fp(2, 3));      // indirect call -> add      -> 5

    fp = multiply;
    printf("%d\n", fp(2, 3));      // indirect call -> multiply -> 6

    return 0;
}
```

Same line `fp(2,3)` printed `5` then `6`. The call site never changed — only the
address inside `fp` did. You have now used a function pointer for its actual
purpose: **deciding at runtime which function a call runs.**

---

## 8. Common mistakes

| Mistake | What happens | Fix |
|---|---|---|
| `fp` is `NULL` when called | Crash (undefined behavior) | Guard: `if (fp) fp(...)` |
| Pointer type ≠ real function type | Garbage args / crash | Make the declared type match exactly |
| Forgetting it's a *call*, writing `fp;` | Nothing runs (just names the address) | Add `()`: `fp(2,3)` |
| Confusing `*fp` with the *result* | `*fp` is the function, not its return value | Call it: `(*fp)(2,3)` or `fp(2,3)` |

---

## 9. What to take into section 05

- Call with `fp(args)` or `(*fp)(args)` — **identical** behavior.
- **Direct call** = address baked into the instruction (compiler knew it).
- **Indirect call** = address fetched from a variable at runtime (`fp`).
- The CPU does the *same* work either way, plus one load for the indirect case.
- The pointer's **type** makes arguments/return values mechanically correct.

You now understand *what* function pointers do and *how* the call works. The
remaining beginner pain is purely **reading and writing the declaration
syntax** — so section 05 is a deep, symbol-by-symbol decoding clinic.

➡️ Continue to **`theory/05-function-pointer-syntax/`**, after running
**`code/04-calling-through-pointers/`**.

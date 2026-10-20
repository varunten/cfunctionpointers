# 01 — Functions in Memory

> **Before you can understand a function *pointer*, you must understand what a
> function actually *is*.** Almost everyone who is confused by function pointers
> is really confused by this: they secretly believe a function is "just code"
> that lives in some magical place the computer knows about. It does not. A
> function is **data sitting at an address**, exactly like a variable. Once you
> truly believe that, function pointers stop being strange.

---

## 1. The big idea (read this twice)

You already accept this sentence without thinking:

> An `int` variable is some bytes stored at an address in memory.

This series exists to make you accept this sentence the same way:

> A **function** is some bytes stored at an address in memory.

The bytes of an `int` encode a *number*.
The bytes of a function encode *machine instructions*.

That is the **only** difference. Both are bytes. Both have addresses. A function
pointer is just a pointer that holds the address of the "function bytes" instead
of the address of the "number bytes."

Hold onto that. Everything else in 18 sections is an elaboration of this one
idea.

---

## 2. What is a function, really?

When you write this C:

```c
int add(int a, int b) {
    return a + b;
}
```

you are *not* creating something that exists while the program runs the way a
variable does. You are giving the **compiler** a recipe. The compiler reads your
recipe and translates it into a sequence of **machine instructions** — the raw
numeric commands your CPU understands.

For `add`, the compiler might produce instructions that conceptually mean:

```text
1. take the first argument
2. take the second argument
3. add them
4. put the result where the caller expects a return value
5. return to whoever called me
```

On a real x86-64 CPU those steps become bytes that look something like:

```text
55              push rbp
48 89 e5        mov  rbp, rsp
89 7d fc        mov  [rbp-4], edi      ; store a
89 75 f8        mov  [rbp-8], esi      ; store b
8b 55 fc        mov  edx, [rbp-4]
8b 45 f8        mov  eax, [rbp-8]
01 d0           add  eax, edx          ; a + b
5d              pop  rbp
c3              ret                     ; return
```

Look at the left column. `55`, `48`, `89`, `e5`, ... — those are **bytes**.
Numbers. Your function `add` *is* that pile of bytes. The compiler had to put
those bytes **somewhere in memory** so the CPU can fetch and execute them.

> **Key realization:** "running a function" means "the CPU starts reading bytes
> at the address where those instructions live, and executes them one by one."

---

## 3. Variables vs functions — the same, but different bytes

Let's put them side by side so the symmetry is impossible to miss.

```text
A variable:                          A function:

int x = 42;                          int add(int a, int b){ return a+b; }

Stored as bytes:                     Stored as bytes:
  2a 00 00 00   (the number 42)        55 48 89 e5 ... c3 (instructions)

Lives at some address:               Lives at some address:
  0x7ffe_1234                          0x0040_1000

You can take its address:            You can take its address:
  &x  ->  0x7ffe_1234                  add ->  0x0040_1000
```

A pointer to the variable holds `0x7ffe1234`.
A pointer to the function holds `0x00401000`.

Same machine, same kind of pointer register, same "a number that names a
location." The CPU does different things with the two addresses (it *reads data*
from one and *jumps to and executes* the other), but the storage is identical:
**an address is an address.**

---

## 4. The memory layout of a running program

When your operating system loads your program and starts it, the program's
memory (its *address space*) is divided into regions called **segments**. You
need to know four of them. Here is the classic picture:

```text
        HIGH ADDRESSES  (e.g. 0x7fff_ffff_ffff)
   +-----------------------------+
   |           STACK             |  <- local variables, function call frames
   |             |               |     grows DOWNWARD as calls nest
   |             v               |
   |                             |
   |   (large unused gap)        |
   |                             |
   |             ^               |
   |             |               |
   |           HEAP              |  <- malloc()/free() memory
   |                             |     grows UPWARD
   +-----------------------------+
   |       BSS  segment          |  <- global/static vars = 0 (uninitialized)
   +-----------------------------+
   |       DATA segment          |  <- global/static vars with initial values
   +-----------------------------+
   |   TEXT (CODE) segment       |  <- *** YOUR FUNCTIONS LIVE HERE ***
   |   add(), multiply(), main() |     machine instructions; usually READ-ONLY
   +-----------------------------+
        LOW ADDRESSES   (e.g. 0x0000_0000_0040_0000)
```

Let's walk each region and where each kind of thing goes.

### TEXT segment (a.k.a. the code segment) — **where functions live**

This is the home of every function you compiled: `main`, `add`, `multiply`,
library functions, all of it. It holds the **machine instruction bytes** from
section 2.

Three properties matter for this whole series:

1. **It has addresses.** Every function starts at a specific address in the text
   segment. `add` might begin at `0x401000`, `multiply` at `0x401020`, etc.
2. **It is usually read-only and executable.** The OS marks these pages
   "execute allowed, write forbidden," so a bug can't overwrite your code and
   malware can't easily inject code. (This is why you can't normally modify a
   function at runtime.)
3. **It is fixed for the life of the program.** Functions don't move around. The
   address of `add` is the same every time you ask for it within one run.

### DATA segment — initialized globals

```c
int counter = 100;      // lives in DATA: bytes for 100 are stored in the file
```

### BSS segment — zero-initialized globals

```c
int table[1000];        // lives in BSS: just reserved space, starts as zeros
```

### HEAP — dynamic memory

```c
int *p = malloc(40);    // the 40 bytes live on the HEAP; p itself may be a local
```

### STACK — locals and the bookkeeping of function calls

```c
void f(void) {
    int local = 5;      // 'local' lives on the STACK while f() runs
}
```

Every time you *call* a function, a **stack frame** is pushed: a small block
holding that call's arguments, local variables, and the *return address* (where
to continue when the function finishes). When the function returns, its frame is
popped. We'll revisit this in section 04, because the return address is itself
"a code address being stored and jumped to" — a cousin of the function pointer.

---

## 5. Two completely different things people call "a function"

This trips up beginners constantly, so name the two ideas explicitly:

```text
  THE CODE (the instructions)            ONE EXECUTION (a call in progress)
  -----------------------------          ---------------------------------
  Lives in the TEXT segment.             Lives on the STACK (a frame).
  Exists for the whole program.          Exists only while the call runs.
  There is exactly ONE copy.             There can be MANY at once
                                         (recursion = many frames of one func).
  Has a fixed address (e.g. 0x401000).   Each frame has its own stack address.
  This is what a function POINTER        This is NOT what a function pointer
  points at.                             points at.
```

A function pointer always points at **the code in the text segment** — the one,
fixed, shared copy of the instructions. It never points at a stack frame. When
you later see `fp = add;`, picture an arrow from `fp` down into the text segment.

```text
   fp  ----------------------+
                             |
   TEXT segment              v
   +--------------------------------+  0x401000
   | machine instructions of add()  |
   +--------------------------------+
```

---

## 6. Why does it matter *where* functions live?

Because the entire premise of a function pointer is:

1. Functions live at **addresses** (this section).
2. Therefore we can **store** those addresses in a pointer (section 03).
3. Therefore we can **pass them around** like any other value (section 07+).
4. Therefore the CPU can be told "go execute whatever is at this address"
   (section 04) — and we get to decide *which* function that is at runtime.

If functions had no addresses, none of this would be possible. They *do* have
addresses, because they are just bytes in the text segment. That's the whole
foundation.

---

## 7. A note on "the CPU's point of view"

The CPU has a special register usually called the **instruction pointer** (`RIP`
on x86-64, `PC` — program counter — on many other CPUs). It holds **the address
of the next instruction to execute**. Running a program is, at the hardware
level, almost entirely this loop:

```text
  loop forever:
      instruction = memory[ RIP ]      ; fetch bytes at the instruction pointer
      RIP = RIP + length(instruction)  ; advance to the next instruction
      execute(instruction)             ; do what it says
```

A normal function **call** is just: "save where I am, then set `RIP` to the
function's address." A function **return** is: "set `RIP` back to the saved
address." A function **pointer call** is: "set `RIP` to whatever address this
pointer holds."

Keep this in your back pocket. In section 04 we will see that calling through a
function pointer is *the exact same hardware operation* as a normal call — only
the source of the address differs.

---

## 8. Diagram: the whole mental model on one screen

```text
   SOURCE CODE                COMPILER             MEMORY (at runtime)
   ------------               --------             -------------------

   int add(int a,int b){  ==>  translates  ==>   TEXT segment
       return a+b;             to machine        +-------------------+ 0x401000
   }                           instructions      | 55 48 89 e5 ...c3 |  <- add
                                                 +-------------------+ 0x401020
   int multiply(int a,    ==>  translates  ==>   | 55 48 89 e5 ...c3 |  <- multiply
       int b){...}             to machine        +-------------------+ 0x401040
                               instructions      | ... main ...      |  <- main
                                                 +-------------------+

   int x = 42;            ==>  reserve     ==>   DATA segment
                               + init             +-------------------+
                                                 |  2a 00 00 00 (42) |  <- x
                                                 +-------------------+
```

The name `add` in your source, after compilation, *means* "the address
`0x401000`". That's the bridge to section 02.

---

## 9. Common misconceptions to unlearn

| Misconception | Reality |
|---|---|
| "Functions aren't stored anywhere; they just *are*." | Functions are bytes stored in the text segment, at addresses. |
| "Code and data are fundamentally different." | At the memory level both are just bytes with addresses. The CPU treats one as instructions and one as data, but storage is identical. |
| "A function only exists when it's running." | The *code* always exists in the text segment. A *call* (stack frame) is what comes and goes. |
| "You can't take the address of a function." | You absolutely can — that's the entire point of section 02. |
| "Function pointers point at the running function." | They point at the fixed code in the text segment, never at a stack frame. |

---

## 10. What to take into section 02

- A function is **bytes (machine instructions)** in the **text segment**.
- Those bytes start at a specific **address**.
- An address is an address — the same kind of thing whether it names an `int` or
  a function.
- The CPU runs code by setting its **instruction pointer** to an address and
  fetching instructions from there.

Next we make the address itself concrete: we'll *print* function addresses, see
that `add` and `&add` give the same thing, and watch different functions sit at
different addresses.

➡️ Continue to **`theory/02-function-addresses/`**, and run the programs in
**`code/01-functions-in-memory/`** first.

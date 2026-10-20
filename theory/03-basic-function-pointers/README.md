# 03 — Basic Function Pointers

> You now know functions live at addresses and a function name decays to its
> address. The natural next question: **can I store that address in a variable?**
> Yes. That variable is a *function pointer*. This section is about what it *is*
> and what it looks like in memory. (Calling through it is section 04; decoding
> the gnarly syntax is section 05.)

---

## 1. The idea, in one line

A **function pointer** is an ordinary variable whose value is the **address of a
function**.

That's it. You already have a pointer that holds the address of an `int`:

```c
int  x  = 42;
int *p  = &x;     // p holds the address of an int
```

A function pointer holds the address of a *function* instead:

```c
int  add(int, int);
int (*fp)(int, int) = add;   // fp holds the address of a function
```

Same concept (a variable storing an address), different *target* (code instead
of data).

---

## 2. Side-by-side: data pointer vs function pointer

```text
   DATA POINTER                         FUNCTION POINTER
   ------------                         ----------------
   int x = 42;                          int add(int,int){...}

   int *p = &x;                         int (*fp)(int,int) = add;

   p holds the address of an int.       fp holds the address of a function.

   *p   reads the int (gives 42).       (*fp)(2,3) or fp(2,3) CALLS the function.

   Memory:                              Memory:
     p ---> [ 42 ]  (data segment       fp ---> [ machine instructions ]
                     or stack)                   (TEXT segment)
```

The shapes match perfectly. The only real differences:

1. **What it points at:** data bytes vs instruction bytes.
2. **What you do with it:** dereference to *read/write a value* vs dereference to
   *call*.
3. **The declaration syntax** is uglier for function pointers, because the type
   has to capture the parameter list and return type (that's section 05).

---

## 3. The memory picture

```c
int add(int a, int b) { return a + b; }

int (*fp)(int, int);   // fp is a variable that can hold a function's address
fp = add;              // now fp holds add's address
```

```text
   STACK (or wherever fp lives)        TEXT segment
   +----------------------+            +----------------------------+ 0x401000
   | fp = 0x401000        |----------->| machine instructions       |
   +----------------------+            | of add()                   |
                                       +----------------------------+

   fp is just bytes holding the number 0x401000.
   That number is "where add's code begins."
```

`fp` itself is a normal variable — it lives on the stack (if local) or in the
data segment (if global), takes up the size of a pointer (usually 8 bytes on a
64-bit machine), and you can copy it, pass it, and reassign it like any value.

```c
fp = add;        // fp -> add's code
fp = multiply;   // now fp -> multiply's code   (just changed the stored address)
```

That single reassignment is the superpower: **the same call site can run
different functions depending on what address `fp` currently holds.**

---

## 4. Reading the declaration (a first taste)

```c
int (*fp)(int, int);
```

Don't panic — section 05 dissects this symbol by symbol. For now, a quick way to
read it:

```text
   int (*fp)(int, int);
   ^^^      ^^^^^^^^^^
   |        |
   |        +-- it points to a function that takes (int, int)
   +----------- ...and returns int

   The (*fp) in the middle says: "fp is a pointer."
```

The parentheses around `*fp` are **mandatory** and are the part everyone forgets.
Here's the trap:

```c
int (*fp)(int, int);   // fp is a POINTER to a function returning int   <-- want this
int  *fp (int, int);   // fp is a FUNCTION returning (int*)             <-- NOT a pointer!
```

Without the parentheses, `*` binds to the return type and you've declared a
*function*, not a *pointer*. The `(*fp)` says "the pointer-ness applies to `fp`
itself." We'll drill this in section 05. For now, just register: **the
parentheses around `*fp` matter.**

---

## 5. Assigning a function to a pointer

All of these are valid and equivalent:

```c
int (*fp)(int, int);

fp = add;     // function name decays to its address (most common style)
fp = &add;    // explicit address-of -- same address (see section 02)
```

You can also initialize at declaration:

```c
int (*fp)(int, int) = add;
```

**The types must match.** A function pointer's type includes its return type and
parameter types. You cannot put `add` (an `int(int,int)`) into a pointer
declared as `void (*)(void)` — the compiler will reject or warn, because the
shapes don't match. This matters later: the type is how the compiler (and the
CPU's calling convention) knows how to pass arguments and interpret the return
value.

```text
   fp's type must equal the function's type:

   int (*fp)(int, int)   <==  int add(int, int)        OK  (matches)
   int (*fp)(int, int)   <==  double f(double)         NO  (mismatch)
```

---

## 6. NULL function pointers

Just like a data pointer, a function pointer can be `NULL`, meaning "points at
no function yet."

```c
int (*fp)(int, int) = NULL;   // not pointing at anything
```

Calling through a `NULL` function pointer is **undefined behavior** (typically a
crash) — exactly like dereferencing a `NULL` data pointer. So the universal
pattern when a function pointer is optional is:

```c
if (fp != NULL)
    fp(2, 3);     // only call if it actually points somewhere
```

You'll see this constantly in callback APIs: "if the user supplied a callback,
call it; otherwise skip." A `NULL` slot is how you say "no behavior here."

---

## 7. The whole idea as a slogan

> A function pointer is **an ordinary pointer that happens to point at executable
> code instead of data.** Storing into it picks *which* function. Calling
> through it runs *that* function.

If section 01 was "functions have addresses" and section 02 was "the name *is*
the address," then section 03 is "**put that address in a box you can change.**"

---

## 8. Diagram: swapping behavior by swapping an address

```text
   fp = add;                          fp = multiply;
   +----------+                       +----------+
   | 0x401000 |--+                    | 0x401040 |--+
   +----------+  |                    +----------+  |
                 v                                  v
        +----------------+                 +----------------+
        | add: a + b     |                 | multiply: a*b  |
        +----------------+                 +----------------+

   Same variable fp, same future call site fp(2,3),
   but the RESULT changed from 5 to 6 -- only because the stored address changed.
```

This is the seed of callbacks, dispatch tables, vtables, plugins — *all* of it.
Everything later is "store the right address in the right box, then call
through it."

---

## 9. What to take into section 04

- A function pointer is a **variable holding a function's address**.
- Declared like `int (*fp)(int, int);` — note the **parentheses around `*fp`**.
- Assign with `fp = add;` (or `&add`); the **types must match**.
- It can be `NULL`; guard before calling.
- Reassigning it changes **which** function a later call will run.

Next: actually **calling** through the pointer, and seeing that it's the same
CPU operation as a normal call — just with the address coming from a register
instead of being baked into the instruction.

➡️ Continue to **`theory/04-calling-through-pointers/`**, after running
**`code/03-basic-function-pointers/`**.

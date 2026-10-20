# 05 — Function Pointer Syntax

> This is the section everyone dreads and nobody needs to. The syntax *looks*
> hostile only because C declarations are read in an unusual order. Once you
> learn the **one rule** for reading them — and a foolproof fallback — you'll
> decode any declaration on sight. We go slow, symbol by symbol.

---

## 1. Why the syntax feels evil

Beginners memorize this shape:

```c
int (*fp)(int, int);
```

and have no idea which piece means what. The parentheses seem random, the `*`
seems misplaced, and it doesn't read left-to-right like English. The good news:
there is a *system*, and it's learnable in ten minutes.

The mental switch you need: **C declarations are designed to mirror how the thing
is *used*, not to be read like prose.** That's the key to everything below.

---

## 2. The "declaration mirrors use" principle

C's rule (going back to the original designers) is:

> A declaration tells you what expression you'd write to get the base type.

Example with a plain pointer:

```c
int *p;
```

reads as: "the expression `*p` has type `int`." So `p` must be a *pointer to
int*. The declaration shows you the *usage* (`*p`) and the resulting *type*
(`int`).

Apply the same idea to function pointers:

```c
int (*fp)(int, int);
```

reads as: "the expression `(*fp)(int, int)` has type `int`." Meaning: if you take
`fp`, dereference it (`*fp`), and call it with two ints (`(*fp)(...)`), you get
an `int`. Therefore `fp` is a *pointer to a function taking (int,int) returning
int*. The declaration literally shows the call you'd make.

This is why the call syntax `(*fp)(2,3)` from section 04 matches the
declaration so neatly — **they're the same shape on purpose.**

---

## 3. The Right-Left Rule (your reliable algorithm)

When in doubt, use this mechanical procedure. It never fails.

> **Start at the identifier. Go RIGHT when you can, LEFT when you must, and let
> parentheses group what binds together. Read `*` as "pointer to", `()` as
> "function returning", `[]` as "array of".**

Let's run it on `int (*fp)(int, int);`

```text
   int (*fp)(int, int);
            ^
            start at the name: "fp is ..."
```

Step 1 — look right of `fp`: there's a `)` closing the inner parens, so we're
boxed in by parentheses. We must go left inside the parens.

Step 2 — left of `fp` inside `( )`: there's a `*`. Read it: "fp is a **pointer
to** ...".

Step 3 — we've used up the inside of the parentheses. Pop out. Now look right of
the `(*fp)` group: there's `(int, int)`. Read it: "... a **function taking
(int, int)** ...".

Step 4 — nothing else to the right. Go left to the remaining base type: `int`.
Read it: "... **returning int**."

Assemble: **"fp is a pointer to a function taking (int, int) returning int."**
Done.

---

## 4. The contrast that explains the parentheses

Why are the parentheses around `*fp` mandatory? Because `()` (call) and `[]`
(subscript) bind **tighter** than `*` (pointer). Without grouping, the `*`
applies last, not first. Watch:

```c
int  *fp (int, int);     // NO parens around *fp
```

Right-Left on this: start at `fp`, go right → `(int, int)` → "function taking
(int,int)"; then left → `*` → "returning pointer to"; then `int`. Result:

> "fp is a **function** taking (int,int) returning **int\***."

That's a *function declaration*, not a pointer! Completely different thing.

```c
int (*fp)(int, int);     // WITH parens
```

The parentheses force `*` to bind to `fp` **first**: "fp is a pointer ..." before
the `(int,int)` call part applies. Result:

> "fp is a **pointer to a function** taking (int,int) returning int."

```text
   int  *fp (int, int);     ->  fp is a FUNCTION returning int*
                                 (the * binds to the return type)

   int (*fp)(int, int);     ->  fp is a POINTER to a function
                                 (the parens bind * to fp first)
```

**The parentheses are not decoration. They change the meaning entirely.** This is
the #1 function-pointer bug. Burn this contrast into memory.

---

## 5. Decoding a gallery of declarations

Practice makes this automatic. Work through each one with the Right-Left rule
before reading the answer.

```c
void (*f)(void);
```
> `f` is a pointer to a function taking no arguments, returning nothing.
> (An "action" with no inputs/outputs — perfect for event handlers.)

```c
double (*g)(double);
```
> `g` is a pointer to a function taking a `double`, returning a `double`.
> (Think `sin`, `cos`, `sqrt`.)

```c
int (*h)(const char *);
```
> `h` is a pointer to a function taking a `const char *`, returning `int`.
> (Think `puts`, or `strlen`-ish callbacks.)

```c
char *(*j)(char *, const char *);
```
> `j` is a pointer to a function taking `(char *, const char *)`, returning
> `char *`. (Shape of `strcpy`/`strcat`.)

```c
void (*signal(int, void (*)(int)))(int);
```
> The infamous `signal` declaration. `signal` is a **function** taking
> `(int, void (*)(int))` and returning **a pointer to a function taking `int`
> returning `void`**. Don't worry if this one hurts — section 06 (typedefs) is
> the cure, and even experts reach for `typedef` here.

---

## 6. Arrays of function pointers

You can have an array whose elements are function pointers — the basis of
dispatch tables (section 11):

```c
int (*ops[3])(int, int);
```

Right-Left: start at `ops`, go right → `[3]` → "array of 3"; can't go further
right (closing paren), go left → `*` → "pointer to"; pop out, right → `(int,int)`
→ "function taking (int,int)"; left → `int` → "returning int".

> "`ops` is an **array of 3 pointers** to functions taking (int,int) returning
> int."

Use it like:

```c
int (*ops[3])(int, int) = { add, subtract, multiply };
int r = ops[2](6, 7);   // calls multiply(6,7) -> 42
```

```text
   ops:  +-----------+-----------+-----------+
         | &add      | &subtract | &multiply |
         +-----------+-----------+-----------+
            ops[0]      ops[1]       ops[2]
                                       |
                          ops[2](6,7)  v  calls multiply -> 42
```

Compare the array-of-pointers above with a *pointer to an array* or a *function
returning a pointer* — the parentheses placement is, again, what disambiguates.

---

## 7. Function pointer as a function parameter

This is how callbacks are received (section 07). A parameter can be a function
pointer:

```c
void each(int *arr, int n, void (*visit)(int));
```

> `each` takes an `int*`, an `int`, and a **pointer to a function taking `int`
> returning `void`** named `visit`. Inside `each`, you call `visit(arr[i])`.

There's a shorthand: in a *parameter list*, you may write a function type and C
silently treats it as a pointer (like arrays decaying):

```c
void each(int *arr, int n, void visit(int));   // 'visit' becomes a pointer too
```

Both declare the same thing. The explicit `(*visit)` form is clearer and far
more common; recognize the shorthand when you see it.

---

## 8. A repeatable "build it up" method for writing them

When you need to *write* a function pointer type, build outward from the name:

1. Write the name: `fp`
2. It's a pointer, so wrap and star it: `(*fp)`
3. Add the parameter list it's called with: `(*fp)(int, int)`
4. Add the return type on the left: `int (*fp)(int, int)`

For an array of them, add `[]` to the name first: `(*fp[3])` then continue. For a
parameter, you usually just drop the name or keep it.

```text
   name          fp
   pointer      (*fp)
   call         (*fp)(int,int)
   return type  int (*fp)(int,int)     <-- done
```

This "grow from the identifier outward" method is the inverse of the Right-Left
*reading* rule, and it's how you'll author declarations without second-guessing.

---

## 9. When to stop fighting the syntax: typedefs

Here's the honest truth professionals live by: **you rarely write raw function
pointer declarations.** You write a `typedef` once and use a clean name
everywhere. Compare:

```c
// raw, repeated everywhere -- error prone:
void register_handler(int (*cb)(int, int));
int  (*get_handler(void))(int, int);

// with a typedef -- readable:
typedef int (*BinaryOp)(int, int);
void     register_handler(BinaryOp cb);
BinaryOp get_handler(void);
```

That is section 06's entire job. So: learn to *read* raw declarations (this
section) so no codebase scares you, and learn to *write* `typedef`s (next
section) so your own code stays clean.

---

## 10. What to take into section 06

- C declarations **mirror usage**: `int (*fp)(int,int)` ⇔ "`(*fp)(int,int)` is
  an `int`."
- Use the **Right-Left rule** when unsure: start at the name, go right then
  left, parentheses group.
- The **parentheses around `*fp` are mandatory**; without them you declare a
  function, not a pointer.
- You can form **arrays** of function pointers and **parameters** that are
  function pointers.
- In real code you'll almost always hide this behind a **`typedef`** — next.

➡️ Continue to **`theory/06-typedefs/`**, after running
**`code/05-function-pointer-syntax/`**.

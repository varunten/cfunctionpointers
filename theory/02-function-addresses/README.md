# 02 — Function Addresses

> In section 01 you accepted that a function is **bytes in the text segment**
> with a starting **address**. This section makes that address a thing you can
> hold, print, and compare. The single most important takeaway: **the name of a
> function, used by itself, evaluates to its address.**

---

## 1. The name of a function *is* an address

Here is the sentence that unlocks everything:

> When you write the bare name of a function (without parentheses), it
> **decays** into a pointer to that function — i.e. it becomes the function's
> address.

Compare with arrays, which you already know:

```c
int arr[5];
arr      // in most contexts, this is &arr[0] -- the address of the first element
```

Functions behave the same way:

```c
int add(int, int);
add      // this is the ADDRESS of add's code -- a pointer to the function
add(2,3) // THIS, with parentheses, is a CALL -- "go execute add"
```

The difference between `add` and `add(2,3)` is the whole game:

```text
   add          ->  a VALUE: the address where add's code starts (e.g. 0x401000)
   add(2, 3)    ->  an ACTION: jump to that address and run it, yielding 5
```

`()` is the **call operator**. No parentheses = no call = you're just naming the
address.

---

## 2. `add` and `&add` are the same address

You can also explicitly take a function's address with `&`:

```c
add      // address of add (name decays to pointer-to-function)
&add     // address of add (explicit "address of")
```

Both produce **the same address**. In `code/02-function-addresses/` you'll print
them and watch them match exactly.

```text
   add   ==>  0x401000
   &add  ==>  0x401000     (identical)
```

### So why do both exist? A subtle type difference.

The *value* (the address) is identical. The *type* differs slightly, and this is
worth knowing so the language lawyers don't surprise you later:

```text
   add    has type:  int(int,int)      -- "function taking (int,int) returning int"
   &add   has type:  int(*)(int,int)   -- "POINTER to function taking (int,int)..."
```

`add` is the function itself (which immediately decays to a pointer in
expressions); `&add` is *already* a pointer to the function. In practice they
are interchangeable when you assign to a function pointer:

```c
int (*fp)(int,int);
fp = add;    // fine: add decays to its address
fp = &add;   // also fine: &add is the same address
```

Use whichever reads better to you. Most C programmers write `fp = add;`. The
`&` is redundant but harmless and some people like it for being explicit. Don't
lose sleep over it — **the address is the same.**

---

## 3. What does the address actually point at?

The **first machine instruction** of the function.

```text
   add  ==  0x401000
                |
                v
   TEXT segment
   +-----------------------------------------+ 0x401000  <-- add starts HERE
   | 55           push rbp                   |
   | 48 89 e5     mov  rbp, rsp              |
   | 89 7d fc     mov  [rbp-4], edi          |
   | ...                                     |
   | c3           ret                        |
   +-----------------------------------------+ 0x401020  <-- next function starts here
```

To "call `add`" the CPU sets its instruction pointer to `0x401000` and starts
fetching. The address is literally "where to start executing." That is *all* a
function address is: **a place to start running instructions.**

---

## 4. Different functions, different addresses

Each function gets its own spot in the text segment, so each has its own
address:

```text
   add       ==>  0x401000
   subtract  ==>  0x401020
   multiply  ==>  0x401040
   divide    ==>  0x401060
```

This is what makes "choose a function at runtime" possible: if I have a variable
that can hold *one* of these addresses, then by changing *which* address it
holds, I change *which* function gets run when I jump to it. That variable is a
function pointer (section 03).

```text
   chosen  ----+        change 'chosen' to pick a different function
               |
               +--> 0x401000  add
                    0x401020  subtract
                    0x401040  multiply
```

---

## 5. Printing a function address (and a gotcha)

To print an address you use `%p`, which expects a `void *`. But here's a pedantic
truth: in standard C, **a function pointer is not guaranteed to be convertible
to `void *`** (on normal desktop platforms it works fine; on exotic
architectures code and data pointers can differ in size). So compilers may warn.

For learning purposes on a normal PC, this is fine and prints the real address:

```c
printf("%p\n", (void *)add);
```

A more portable trick is to print it as a function pointer using a cast, but
`%p` + `(void*)` is what you'll see 99% of the time. Just know *why* the warning
exists if it appears: historically, not all machines promised "code addresses
and data addresses are the same size/format." On your laptop, they are.

---

## 6. Addresses change between runs (ASLR) — but stay consistent within a run

If you run `code/02-function-addresses/01_print_addresses.c` twice, you may see
**different** numbers each time. That's **ASLR** (Address Space Layout
Randomization) — the OS deliberately loads your program at a random base each
run to make attacks harder.

This does **not** break function pointers. What matters is:

- Within a single run, `add`'s address is **fixed and consistent**.
- The *relationships* hold: `&add == add`, two different functions have two
  different addresses, etc.

Think of it like a hotel: the room *numbers* (relative offsets) are always the
same, but the hotel might be built on a different street (base address) each
time.

---

## 7. Diagram: name vs call, one more time

```text
   SOURCE                MEANING                         CPU ACTION
   ------                -------                         ----------
   add                   the address 0x401000            (none -- it's just a value)
   &add                  the address 0x401000            (none -- same value)
   add(2, 3)             call: run code at 0x401000      set RIP=0x401000, execute
                         with arguments 2 and 3,
                         get 5 back
```

If you can read that table fluently, you understand function addresses. The rest
of the series is "store that address somewhere and call through it later."

---

## 8. Mini-FAQ

**Q: Is `add` a pointer?**
By itself in an expression, it *decays into* a pointer to function. You can
think of "naming a function = getting its address," just like naming an array
gets you the address of its first element.

**Q: Why does `add(2,3)` work but `add` alone doesn't run anything?**
Because `()` is the call operator. Without it, you only mention the address. With
it, you tell the CPU to jump there.

**Q: Can I do `add + 1` to get the next function?**
No. Function pointer arithmetic is not allowed in standard C (the language has no
defined "size of a function"). Functions aren't an array you can stride through.

**Q: Are `add` and `&add` ever different?**
Their *value* (the address) is the same. Their *type* differs:
`int(int,int)` vs `int(*)(int,int)`. For assignment to a function pointer it
doesn't matter.

---

## 9. What to take into section 03

- A bare function name = **the function's address**.
- `add` and `&add` = **the same address** (slightly different types).
- The address points at the **first instruction** of the function.
- Different functions = different addresses; that's what lets us *choose*.
- Now we need a **variable that can hold a function address** so we can store
  and swap which function to run. That variable is the **function pointer**.

➡️ Continue to **`theory/03-basic-function-pointers/`**, after running
**`code/02-function-addresses/`**.

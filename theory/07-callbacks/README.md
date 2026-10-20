# 07 — Callbacks

> This is the first *big* application — the reason function pointers earn their
> keep. A **callback** is a function you hand to *other* code so that *it* can
> call *your* code back at the right moment. It flips the usual direction of
> control, and once you see it, you'll recognize it in every library you've ever
> used.

---

## 1. The normal direction of control

Usually *your* code is in charge. You call into a library; the library does its
thing and returns. Control flows **down** into the library and back **up** to
you:

```text
   YOUR CODE                LIBRARY
   ---------                -------
   result = sqrt(2.0);  -->  sqrt runs
                        <--  returns 1.414...
   (you keep going)
```

You decide *when* and *what* to call. The library is passive — it waits to be
called.

---

## 2. Inversion of control: the library calls *you*

A callback turns this around. You give the library a **function pointer**, and
the library calls it at a moment *it* chooses. Control flows the other way —
"don't call us, we'll call you."

```text
   YOUR CODE                          LIBRARY
   ---------                          -------
   library_do_work(my_callback);  -->  library starts working
                                       ...at the right moment...
   my_callback runs  <----------------  library CALLS BACK into your function
                     ---------------->  (your callback returns)
                                       library finishes
                                  <--  returns to you
```

This is **inversion of control**: the library owns the timing and the loop; you
supply the *behavior*. The function you handed over is the **callback**.

```text
        Library
           |
           |  "at the right moment, run whatever the user gave me"
           v
        User Function   (the callback)
```

---

## 3. Why is this powerful? Separation of "what" from "when/how"

A callback lets a library be written *once*, without knowing what you'll want to
do, and still do exactly what you want. The library handles the **mechanism**
(iterating, waiting, sorting, polling), and you supply the **policy** (what to do
with each item, how to compare, what to do on an event).

Classic example — "do something to every element of an array." The *looping* is
always the same; the *something* differs every time:

```c
typedef void (*Visitor)(int value);

void for_each(int *arr, int n, Visitor visit) {
    for (int i = 0; i < n; i++)
        visit(arr[i]);          // <-- the callback: library calls YOUR code
}
```

Now the same `for_each` can print, sum, or count, depending on the callback you
pass:

```c
void print_it(int v) { printf("%d ", v); }
...
for_each(arr, n, print_it);     // prints each element
```

The library author wrote the loop **once** and never has to anticipate "print"
vs "sum" vs "validate." You inject that.

```text
   for_each (the MECHANISM, written once)
   +-------------------------------+
   | for i in 0..n:                |
   |     visit(arr[i])  -----------|----> YOUR callback (the POLICY)
   +-------------------------------+        print? sum? count? you decide
```

---

## 4. Step-by-step: what actually happens at runtime

Trace `for_each(arr, 3, print_it)` with `arr = {10, 20, 30}`:

```text
   1. main calls for_each, passing the ADDRESS of print_it as 'visit'.
        visit now holds print_it's address.

   2. for_each enters its loop:
        i=0: visit(arr[0]) -> indirect call to print_it(10) -> prints "10 "
        i=1: visit(arr[1]) -> indirect call to print_it(20) -> prints "20 "
        i=2: visit(arr[2]) -> indirect call to print_it(30) -> prints "30 "

   3. loop ends, for_each returns to main.
```

Notice step 2 is just the **indirect call** from section 04, happening inside the
library, targeting *your* function. A callback is nothing more exotic than "a
function pointer that someone else calls." All the machinery you already learned.

---

## 5. The `void *user_data` parameter — the missing piece

Real callbacks almost always need *context*. Suppose you want `for_each` to
**sum** the elements. Your callback needs somewhere to accumulate the total — but
the callback's signature is fixed by the library. How do you smuggle in your own
state?

The universal answer: the library passes an extra **`void *` "user data"**
pointer straight through to your callback, untouched. You put whatever you want
behind it.

```c
typedef void (*Visitor)(int value, void *user_data);

void for_each(int *arr, int n, Visitor visit, void *user_data) {
    for (int i = 0; i < n; i++)
        visit(arr[i], user_data);     // library passes your context back to you
}
```

Your summing callback:

```c
void accumulate(int v, void *user_data) {
    int *total = user_data;   // we know it's really an int*
    *total += v;
}

int total = 0;
for_each(arr, n, accumulate, &total);   // hand the library a pointer to our total
printf("sum = %d\n", total);
```

```text
   main owns 'total'                 for_each (doesn't know/care what user_data is)
   +-------------+                   +------------------------------------------+
   | total = 0   |<------------------|  passes user_data straight to your        |
   +------^------+   user_data ptr   |  callback every time: visit(x, user_data) |
          |                          +------------------------------------------+
          |  accumulate() writes through user_data back into your total
          +-- *total += v
```

> **`void *user_data` is how C callbacks carry state.** The library treats it as
> an opaque pointer (it never looks inside); your callback casts it back to the
> real type. This single convention is everywhere: `qsort_r`, pthreads, GUI
> toolkits, event loops, you name it. (In C++ a lambda capture or `std::function`
> does this for you; in C *you* do it with `void *`.)

---

## 6. Optional callbacks and the NULL guard

Libraries often make callbacks optional: "call this if you provided one." That's
the `NULL` guard from section 03, now in its natural habitat:

```c
void download(const char *url, void (*on_progress)(int percent)) {
    for (int p = 0; p <= 100; p += 25) {
        /* ... do a chunk of work ... */
        if (on_progress)            // only call if the user supplied a callback
            on_progress(p);
    }
}
```

Pass `NULL` and you simply opt out of progress notifications. This is why
function pointers being nullable matters: `NULL` cleanly means "no behavior
here."

---

## 7. Synchronous vs asynchronous callbacks (a heads-up)

There are two flavors, and the distinction matters enormously for *lifetime*
(section 15 goes deeper):

- **Synchronous** callback: called *during* the function you passed it to, before
  that function returns (like `for_each` and `qsort`). Easy: your data is
  guaranteed alive the whole time.

- **Asynchronous** callback: stored now, called *later* (an event fires, a timer
  expires, a network reply arrives). Harder: the data you pointed `user_data` at
  must still be valid when the callback eventually runs. A pointer to a local
  variable that has since gone out of scope = disaster.

```text
   SYNCHRONOUS                         ASYNCHRONOUS
   -----------                         ------------
   for_each(arr, n, cb, &ctx);         on_click(button, cb, &ctx);
       cb runs NOW, inside for_each        cb runs LATER, when clicked
   &ctx is obviously still alive        &ctx must STILL be alive then!
```

For now we use synchronous callbacks (safe). Keep the async lifetime warning in
mind; it's a real source of bugs and we'll address it in API design.

---

## 8. Designing a callback-based function (a recipe)

When you write a function that takes a callback, decide:

1. **Signature** of the callback — what arguments does it receive? Use a
   `typedef` (section 06) so it's readable.
2. **`void *user_data`** — include one so callers can pass context. Pass it
   through untouched.
3. **Optional or required?** If optional, accept `NULL` and guard the call.
4. **When** is it called — once? per item? on an event? Document it.
5. **Return value** of the callback — does it control anything? (e.g. "return
   nonzero to stop iterating"). A common, powerful pattern.

A polished example combining these:

```c
typedef int (*Visitor)(int value, void *user_data);  // return nonzero to stop

void for_each(int *arr, int n, Visitor visit, void *user_data) {
    if (!visit) return;                       // (4) optional
    for (int i = 0; i < n; i++)
        if (visit(arr[i], user_data) != 0)    // (5) callback can stop the loop
            break;
}
```

This tiny function is, conceptually, how `nftw`, many `walk`/`iterate` APIs, and
countless internal helpers are shaped.

---

## 9. You've been using callbacks all along

You've passed callbacks without thinking of them as such. The big one is coming
next section: `qsort` takes a *comparison callback*. Others you'll meet:

- `qsort` / `bsearch` — comparison function (section 08).
- `pthread_create` — the function the new thread runs.
- `atexit` — a function to run at program exit.
- `signal` — a handler to run when a signal arrives.
- GUI toolkits — "on click, call this."
- `nftw` — call this for every file while walking a directory tree.

Spotting the pattern is half the skill; section 17 is dedicated to reading such
code fluently.

---

## 10. What to take into section 08

- A **callback** is a function pointer you give to other code so it can call
  *you* — **inversion of control**.
- It separates **mechanism** (the library's loop/timing) from **policy** (your
  behavior).
- Carry per-call state with a **`void *user_data`** parameter the library passes
  through untouched.
- Make callbacks **optional** with `NULL` + a guard; consider a **return value**
  to control the host (e.g. stop iterating).
- Mind **lifetime**: synchronous is safe; asynchronous needs the data to outlive
  the call.

Next we study the most famous callback in the C standard library — `qsort`'s
comparator — and watch behavior get injected into sorting.

➡️ Continue to **`theory/08-comparators-and-sorting/`**, after running
**`code/07-callbacks/`**.

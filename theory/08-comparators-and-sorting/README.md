# 08 — Comparators and Sorting

> Now we study the most famous callback in all of C: the **comparison function**
> you pass to `qsort`. It's the perfect case study because it answers "why would
> a library *need* a callback?" so clearly. `qsort` knows *how to sort* but has
> no idea *how to compare your data* — so you inject that knowledge as a
> function pointer.

---

## 1. The problem `qsort` faces

`qsort` is in the standard library and must sort **anything**: arrays of `int`,
of `double`, of strings, of your custom structs, ascending or descending, by any
field. But the library author wrote `qsort` *years before your program existed*
and cannot possibly know:

- How big is one element? (an `int` is 4 bytes, your struct might be 64)
- Which comes first — how do you compare two of your elements?

`qsort` solves the first with a `size` parameter and the second with a
**comparison callback**. It owns the *algorithm* (the actual sorting); you own
the *comparison* (the ordering policy). Pure separation of mechanism from policy
— exactly section 07's lesson, in the standard library.

---

## 2. The signature of `qsort`

```c
void qsort(void *base,        // pointer to the start of the array
           size_t nmemb,      // number of elements
           size_t size,       // size of ONE element, in bytes
           int (*compare)(const void *, const void *));  // the callback
```

That last parameter is the comparator: **a pointer to a function taking two
`const void *` and returning an `int`.** Read it with the Right-Left rule from
section 05 if it looks scary — it's "pointer to function taking
(const void*, const void*) returning int."

Why `const void *`? Because `qsort` must work with *any* element type, it can't
name a concrete type. `void *` means "pointer to something — I won't presume
what." `const` promises the comparator won't modify the elements while comparing.

---

## 3. The comparator contract

Your comparison function receives **pointers to two elements** and must return:

```text
   compare(a, b) returns:
       < 0   if  *a should come BEFORE *b   (a is "less")
       = 0   if  *a and *b are equal in order
       > 0   if  *a should come AFTER  *b   (a is "greater")
```

This three-way "negative / zero / positive" convention is the same idea as
`strcmp`. `qsort` calls your comparator many times, on various pairs, and uses
the sign of the result to arrange elements.

> You are not sorting. You are *answering questions* about order. `qsort` does
> the sorting and simply asks you "which of these two comes first?" over and
> over.

---

## 4. Writing a comparator for `int`

Because the callback receives `const void *`, your first job inside is to cast
the pointers back to the real type, then dereference:

```c
int compare_int(const void *a, const void *b) {
    int x = *(const int *)a;   // cast void* -> int*, then read the int
    int y = *(const int *)b;
    if (x < y) return -1;
    if (x > y) return  1;
    return 0;
}
```

A common shorthand you'll see (valid for `int` when there's no overflow risk):

```c
int compare_int(const void *a, const void *b) {
    return (*(const int *)a) - (*(const int *)b);   // negative/zero/positive
}
```

> **Caution:** the subtraction trick can *overflow* for large/negative values
> (e.g. `INT_MIN - INT_MAX`), giving the wrong sign. The explicit
> `if (<) / if (>)` version is always safe. Know both; prefer the safe one.

Use it:

```c
int arr[] = { 5, 2, 9, 1, 7 };
qsort(arr, 5, sizeof(int), compare_int);   // arr is now {1,2,5,7,9}
```

```text
   qsort(arr, 5, sizeof(int), compare_int)
          |    |       |            |
          |    |       |            +-- HOW to compare (your callback)
          |    |       +--------------- size of one element (4 bytes)
          |    +----------------------- how many elements (5)
          +---------------------------- where the array starts
```

---

## 5. Step-by-step: how `qsort` uses the callback

Picture `qsort` sorting `{5, 2, 9}`. Internally it runs some algorithm
(quicksort, mergesort — implementation-defined) and whenever it needs to know
the order of two elements, it **calls back into your comparator**:

```text
   qsort begins. It needs to order elements...

   "is element[0] before element[1]?"   ->  compare(&5, &2)
        your code: 5 - 2 = 3 (positive) ->  "5 comes AFTER 2"
        qsort: swaps them                ->  {2, 5, 9}

   "is element[1] before element[2]?"   ->  compare(&5, &9)
        your code: 5 - 9 = -4 (negative)->  "5 comes BEFORE 9"
        qsort: leaves them               ->  {2, 5, 9}

   ...and so on, until sorted.
```

Every comparison is an **indirect call** (section 04) from inside the library to
your function. `qsort` supplies the *when* and *which pairs*; you supply the
*verdict*. You never see qsort's loop; qsort never sees your comparison logic.
They meet only through the function pointer.

```text
   qsort (the ALGORITHM)                  YOU (the ORDERING)
   +-----------------------+              +------------------------+
   | pick pairs, ask order |--compare(a,b)| return -, 0, or +       |
   | swap based on answer  |<-------------|                        |
   | repeat until sorted   |              +------------------------+
   +-----------------------+
```

---

## 6. Changing behavior by changing the callback

This is where the power becomes obvious. Want **descending** order? Don't touch
`qsort` — just flip the comparator:

```c
int compare_int_desc(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    if (x > y) return -1;     // bigger comes first now
    if (x < y) return  1;
    return 0;
}
qsort(arr, 5, sizeof(int), compare_int_desc);   // {9,7,5,2,1}
```

Want to sort an array of **structs by a field**? Write a comparator that looks at
that field:

```c
typedef struct { char name[32]; int age; } Person;

int by_age(const void *a, const void *b) {
    const Person *pa = a, *pb = b;
    return pa->age - pb->age;        // ascending by age
}

int by_name(const void *a, const void *b) {
    const Person *pa = a, *pb = b;
    return strcmp(pa->name, pb->name);   // alphabetical by name
}

qsort(people, count, sizeof(Person), by_age);    // sort by age
qsort(people, count, sizeof(Person), by_name);   // re-sort by name
```

Same `qsort`, same data, completely different orderings — selected purely by
*which function pointer you pass*. The behavior is **injected**.

---

## 7. Casting `void *`: the one tricky bit

The only thing that trips people up is the `void *` dance. Remember the chain:

```text
   const void *a            a points to one element, type unknown to qsort
        |  cast to the real pointer type
        v
   const int *p = a;        now the compiler knows it's an int
        |  dereference
        v
   int value = *p;          the actual element value
```

For structs it's the same, you just cast to your struct pointer and read a field.
The `void *` is the price of `qsort` being able to sort *anything*: it forgets
the type, and you remind it inside the comparator.

---

## 8. `bsearch` — the same comparator, reused

The standard library's binary search, `bsearch`, takes the **exact same
comparator shape**:

```c
void *bsearch(const void *key, const void *base,
              size_t nmemb, size_t size,
              int (*compare)(const void *, const void *));
```

So a comparator you wrote for sorting can be reused for searching the (now
sorted) array. One ordering policy, two algorithms. This is the payoff of
designing around a clean callback contract — different mechanisms share the same
injected behavior.

---

## 9. Why this matters beyond sorting

`qsort` is the canonical teaching example, but the *pattern* is universal:

- A general algorithm (sort, search, traverse, filter, reduce) is written once.
- The type-specific or policy-specific decision is supplied as a callback.
- Changing behavior = changing the function pointer, not the algorithm.

You'll see exactly this shape in event systems (section 09), dispatch tables
(11), and especially when reading real code (17). `qsort` is your Rosetta Stone:
once you fully get *why* it needs a comparator, you understand callback-based
APIs in general.

---

## 10. What to take into section 09

- `qsort` owns the **algorithm**; your **comparator** owns the **ordering** —
  classic mechanism/policy split.
- The comparator gets two **`const void *`**; cast them to the real type, then
  return **negative / zero / positive**.
- Prefer explicit `if (<)/if (>)` over subtraction to avoid overflow.
- Change ordering (ascending, descending, by-field) by changing **which
  comparator you pass** — `qsort` is untouched.
- `bsearch` reuses the same comparator — clean contracts pay off.

Next we generalize "register a function to be called when something happens" into
a full **event system** with multiple listeners.

➡️ Continue to **`theory/09-event-systems/`**, after running
**`code/08-comparators-and-sorting/`**.

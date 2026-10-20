# 14 — VTables in C

> Section 13 gave every object its own copy of all its method pointers. For a
> thousand dogs that's a thousand redundant copies of the same three addresses —
> wasteful and bug-prone. The fix is the **vtable** (virtual table): *one* shared
> table of function pointers per type, and each object just holds *one* pointer
> to it. This is precisely how C++ virtual functions work, and how huge C
> systems (the Linux kernel, COM, GTK) implement polymorphism.

---

## 1. The problem with per-object method pointers

Recall section 13's object:

```c
struct animal {
    const char *name;
    void (*speak)(struct animal *self);
    void (*move)(struct animal *self);
    int  (*legs)(struct animal *self);
};
```

Every single dog stores `dog_speak`, `dog_move`, `dog_legs` — the *same three
addresses* in every dog. With 1000 dogs:

```text
   dog #1: name="Rex"   speak=dog_speak move=dog_move legs=dog_legs
   dog #2: name="Buddy" speak=dog_speak move=dog_move legs=dog_legs
   dog #3: name="Max"   speak=dog_speak move=dog_move legs=dog_legs
   ...                  ^^^^^^^^^^^^^^^ ^^^^^^^^^^^^^ ^^^^^^^^^^^^^
   1000 dogs            the SAME pointers, copied 1000 times -- wasteful
```

The method pointers depend on the *type* (dog), not the *instance* (Rex vs
Buddy). So they shouldn't live in each instance. They should live **once, per
type**, and each instance should point to that shared set.

---

## 2. The vtable: one shared table per type

Split the struct in two:

1. A **vtable**: a struct holding all the method pointers for a type. There's
   **one instance of it per type** (one for dogs, one for cats), usually a
   `static const`.
2. The **object**: holds its own data plus a **single pointer to its type's
   vtable**.

```c
struct animal;   /* forward */

/* The vtable type: just the method pointers. */
struct animal_vtable {
    void (*speak)(struct animal *self);
    void (*move)(struct animal *self);
    int  (*legs)(struct animal *self);
};

/* The object: data + ONE pointer to its vtable. */
struct animal {
    const struct animal_vtable *vtable;   /* shared table for this object's type */
    const char *name;                     /* per-instance data */
};
```

```text
   dog #1 "Rex"  ---vtable--+
   dog #2 "Buddy"---vtable--+----> dog_vtable (ONE copy, shared)
   dog #3 "Max"  ---vtable--+        +------------------+
                                     | speak = dog_speak|
                                     | move  = dog_move |
                                     | legs  = dog_legs |
                                     +------------------+

   cat #1 "Felix"---vtable-------> cat_vtable (ONE copy, shared)
                                     +------------------+
                                     | speak = cat_speak|
                                     | move  = cat_move |
                                     | legs  = cat_legs |
                                     +------------------+
```

Now each object stores just **one pointer** (to its vtable) instead of N method
pointers. All dogs share `dog_vtable`. Memory saved, and there's a single place
that defines "what a dog does."

---

## 3. The vtables themselves (defined once)

```c
void dog_speak(struct animal *s){ printf("%s: Woof\n", s->name); }
void dog_move(struct animal *s) { printf("%s runs\n", s->name); }
int  dog_legs(struct animal *s) { (void)s; return 4; }

/* ONE shared vtable for all dogs: */
static const struct animal_vtable dog_vtable = {
    .speak = dog_speak,
    .move  = dog_move,
    .legs  = dog_legs,
};

/* a constructor just points the object at the shared vtable */
struct animal make_dog(const char *name) {
    struct animal a;
    a.vtable = &dog_vtable;   // share, don't copy
    a.name   = name;
    return a;
}
```

Make a thousand dogs and they all set `a.vtable = &dog_vtable` — pointing at the
*same* table. The method addresses exist in exactly one place.

---

## 4. Virtual dispatch: calling through the vtable

To call a method, go through the vtable pointer:

```c
a->vtable->speak(a);   // look up 'speak' in a's vtable, call it, pass a as self
```

Read that right to left in terms of the diagram:

```text
   a->vtable          -> the shared table for a's type (e.g. dog_vtable)
   a->vtable->speak   -> the speak function pointer in that table (dog_speak)
   a->vtable->speak(a)-> call it, passing the object as self
```

This two-step lookup — *object → vtable → method* — is called **virtual
dispatch**. It's one extra pointer hop compared to section 13 (object → method),
in exchange for sharing the table. That hop is exactly what C++ does for a
`virtual` call.

```text
   VIRTUAL DISPATCH (two hops)

   object a            vtable (shared)         function
   +----------+        +---------------+       +--------------+
   | vtable --|------->| speak --------|------>| dog_speak    |
   | name     |        | move          |       +--------------+
   +----------+        | legs          |
        |              +---------------+
        | passed as 'self'
        +-------------------------------------> dog_speak(a)
```

---

## 5. Step-by-step: `a->vtable->speak(a)` for a dog

```text
   1. a is a struct animal whose vtable field points at dog_vtable.
   2. a->vtable      -> read the pointer: it's &dog_vtable.
   3. a->vtable->speak -> read the 'speak' field of dog_vtable: it's dog_speak.
   4. (...)(a)       -> indirect call to dog_speak, passing a as self.
   5. dog_speak prints "<a->name>: Woof".
```

For a cat, step 2 yields `&cat_vtable`, step 3 yields `cat_speak`, and step 5
meows. **Same call expression, different vtable, different method.** The object
"knows what it is" purely through its vtable pointer.

---

## 6. This *is* C++ (and Java, and C#)

When you compile this C++:

```cpp
struct Animal { virtual void speak(); virtual void move(); virtual int legs(); };
struct Dog : Animal { /* overrides */ };

Animal *a = new Dog();
a->speak();
```

the compiler generates almost exactly the C above:

- Each polymorphic object gets a hidden **vptr** (vtable pointer) as its first
  member — same as our `vtable` field.
- Each class has one **vtable** (the compiler builds it) — same as our
  `dog_vtable`.
- `a->speak()` compiles to "load the vptr, load the speak slot, call it with `a`
  as the hidden `this`" — same as `a->vtable->speak(a)`.

> When people say "C++ virtual functions cost a pointer per object and an extra
> indirection per call," *this is the picture they're describing.* You are now
> looking at the actual machinery. C++ didn't invent magic; it automated the
> vtable pattern you just built.

---

## 7. "Overriding" and inheritance with vtables

Inheritance + embedding (section 13's trick) combines beautifully with vtables.
A derived type embeds the base as its first member and supplies its own vtable
(possibly reusing some base methods, overriding others):

```text
   base vtable:   speak=base_speak  describe=base_describe
   derived vtable:speak=derived_speak  describe=base_describe   <- reused base method!
                          ^^^^^^^^^^^^                ^^^^^^^^^^^^^
                          OVERRIDDEN                  INHERITED (same pointer)
```

"Overriding" = the derived vtable puts a different function pointer in a slot.
"Inheriting" = the derived vtable copies the base's pointer for that slot. That's
all virtual inheritance is at the machine level: choosing which function pointer
goes in each table slot. The whole edifice of OOP reduces to *tables of function
pointers per type.*

---

## 8. `const` and where the vtable lives

Notice `static const struct animal_vtable dog_vtable`. The `const` matters:

- The vtable never changes at runtime, so making it `const` lets the compiler put
  it in **read-only memory** (the data/rodata segment) — safe from accidental
  writes, shareable.
- Objects hold a `const struct animal_vtable *`, promising not to modify the
  shared table.

This mirrors reality: C++ vtables live in read-only memory too. (Corrupting a
vtable pointer is a classic exploitation target precisely because virtual calls
trust it — another reason real systems keep vtables read-only.)

---

## 9. Trade-offs

| Aspect | Per-object pointers (sec 13) | VTable (sec 14) |
|---|---|---|
| Memory per object | N method pointers | 1 vtable pointer |
| Memory per type | (none extra) | 1 shared vtable |
| Call cost | 1 indirection | 2 indirections (obj→vtable→method) |
| Best when | very few objects, or per-object behavior | many objects of few types (the usual case) |
| Runtime per-object tweak | easy (edit the object's pointer) | affects all sharers (edit the shared table) |

The vtable wins overwhelmingly for the common case (many instances, few types) —
which is why every OOP language uses it. Per-object pointers are occasionally
handy when individual objects genuinely need unique behavior.

---

## 10. What to take into section 15

- A **vtable** is **one shared table of method pointers per type**; each object
  holds a **single pointer** to its type's vtable.
- **Virtual dispatch** = `object → vtable → method` (two hops), passing the
  object as `self`.
- This is **exactly how C++/Java/C# virtual methods work** — the vptr, the
  vtable, the extra indirection are all right here.
- Keep vtables `static const` (read-only); "override" = different slot,
  "inherit" = copied slot.

We've now built objects and virtual dispatch from scratch. The remaining
sections turn outward: how to **design APIs** that expose function pointers well
(15), how **plugins** use function tables to extend programs (16), how to **read**
real function-pointer code (17), and **capstone projects** (18).

➡️ Continue to **`theory/15-api-design/`**, after running
**`code/14-vtables-in-c/`**.

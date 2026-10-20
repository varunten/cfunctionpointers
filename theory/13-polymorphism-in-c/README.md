# 13 — Polymorphism in C

> C has no `class`, no `virtual`, no inheritance keyword. Yet the Linux kernel,
> GTK, COM, and countless C libraries are thoroughly object-oriented. The secret
> is function pointers. This section shows how to give different "types" a
> **common interface** with **different implementations** — polymorphism — using
> nothing but structs and function pointers.

---

## 1. What "polymorphism" actually means

Strip away the jargon. **Polymorphism** = "many forms": the ability to call the
*same operation* on *different kinds of things* and have each do the *right
thing* for its kind.

```text
   speak(dog)  -> "Woof"
   speak(cat)  -> "Meow"
   speak(bird) -> "Tweet"
```

You write `animal->speak(animal)` once, and *what runs* depends on whether the
animal is a dog, cat, or bird. The caller doesn't need to know which — it just
asks the animal to speak. That's polymorphism, and it's incredibly powerful for
writing code that works with "any animal" without a giant `if/switch` on type.

---

## 2. The non-polymorphic baseline (the pain)

Without polymorphism, you'd branch on a type tag everywhere:

```c
typedef enum { DOG, CAT, BIRD } AnimalKind;

void speak(AnimalKind kind) {
    switch (kind) {
        case DOG:  printf("Woof\n"); break;
        case CAT:  printf("Meow\n"); break;
        case BIRD: printf("Tweet\n"); break;
    }
}
```

Every operation (`speak`, `move`, `eat`, ...) needs its own `switch`. Add a new
animal and you edit *every* switch. This is the same problem dispatch tables
solved (section 11) — and the solution is related: attach the behavior to the
*thing* instead of branching on its tag.

---

## 3. The core trick: put a function pointer *inside* the struct

Instead of an external `switch`, give each object a **function pointer to its own
behavior**. The object *carries* its implementation.

```c
struct animal {
    const char *name;
    void (*speak)(struct animal *self);   // each animal points to its own speak
};
```

```text
   struct animal (a dog)              struct animal (a cat)
   +----------------------+           +----------------------+
   | name  = "Rex"        |           | name  = "Whiskers"   |
   | speak = ---------+   |           | speak = ---------+   |
   +------------------|---+           +------------------|---+
                      v                                  v
              dog_speak()                          cat_speak()
              prints "Woof"                        prints "Meow"
```

Now "make this animal speak" is `a->speak(a)` — an indirect call through the
function pointer the object carries. A dog runs `dog_speak`; a cat runs
`cat_speak`. **Same call, different behavior, selected by the object itself.**

---

## 4. The `self` parameter: how methods know their data

Notice `speak` takes `struct animal *self`. This is the manual version of what
C++/Java/Python give you implicitly (`this` / `self`). Because a C function isn't
attached to an object, you must **pass the object in** so the function can access
its data:

```c
void dog_speak(struct animal *self) {
    printf("%s says Woof\n", self->name);   // uses self to get the name
}
```

```text
   a->speak(a)
      |     |
      |     +-- pass the object as 'self' so the method can read its fields
      +-------- call the function pointer the object carries
```

> **Every "method" in C-style OOP takes the object as its first parameter.** This
> is the foundational convention. `obj->method(obj, args...)`. It looks
> redundant (you name `obj` twice), but it's exactly what C++ does under the hood
> — the compiler just hides the first argument. Seeing it explicitly here
> demystifies how *any* OOP works.

---

## 5. Building it: constructors that wire up the pointers

To make a dog, you allocate/initialize an `animal` and point its `speak` at
`dog_speak`. A "constructor" function does this:

```c
void dog_speak(struct animal *self)  { printf("%s says Woof\n", self->name); }
void cat_speak(struct animal *self)  { printf("%s says Meow\n", self->name); }

struct animal make_dog(const char *name) {
    struct animal a;
    a.name  = name;
    a.speak = dog_speak;     // wire the behavior
    return a;
}
struct animal make_cat(const char *name) {
    struct animal a;
    a.name  = name;
    a.speak = cat_speak;
    return a;
}
```

Use them uniformly:

```c
struct animal zoo[] = { make_dog("Rex"), make_cat("Whiskers"), make_dog("Buddy") };

for (int i = 0; i < 3; i++)
    zoo[i].speak(&zoo[i]);   // polymorphism! each does the right thing
```

```text
   for each animal:  animal.speak(&animal)
        Rex      -> dog_speak  -> "Rex says Woof"
        Whiskers -> cat_speak  -> "Whiskers says Meow"
        Buddy    -> dog_speak  -> "Buddy says Woof"
```

The loop has **no `switch`, no type tag, no knowledge of dog vs cat.** It just
asks each animal to speak. Adding a `Bird` means writing `bird_speak` and
`make_bird` — the loop never changes. *That's* the win.

---

## 6. Multiple methods: a common interface

Real objects have several operations. Just add more function pointers; together
they form the object's **interface** (its set of supported operations):

```c
struct animal {
    const char *name;
    void (*speak)(struct animal *self);
    void (*move)(struct animal *self);
    int  (*legs)(struct animal *self);
};
```

Any animal supports `speak`, `move`, `legs` — that's the *interface*. A dog and a
bird implement them differently — those are the *implementations*. Code written
against the interface (`struct animal *`) works with *any* animal, present or
future. This is "program to an interface, not an implementation," achieved with
plain structs and function pointers.

```text
   INTERFACE (the struct's function-pointer fields)
   +----------------------------------+
   | speak  move  legs                |   <- every animal promises these
   +----------------------------------+
            |          |          |
       dog impl    cat impl   bird impl    <- each fills them differently
```

---

## 7. "Inheritance" via struct embedding

C even fakes inheritance. Put a base struct as the *first member* of a derived
struct; because C guarantees the first member shares the struct's starting
address, a `Derived*` can be safely treated as a `Base*`:

```c
struct animal { /* base: name + method pointers */ };

struct dog {
    struct animal base;     // MUST be first
    int favorite_toy;       // dog-specific data
};
```

A `struct dog *` can be cast to `struct animal *` (they start at the same
address), so generic animal code works on dogs, while dog-specific code uses the
extra fields. This "base struct first" trick is *exactly* how the Linux kernel
(`struct kobject` embedding), GTK's `GObject`, and many C frameworks implement
inheritance.

```text
   struct dog in memory:
   +---------------------------+  <- &dog  ==  &dog.base  (same address!)
   |  struct animal base       |     so (struct animal*)dog is valid
   |    name, speak, move,legs |
   +---------------------------+
   |  int favorite_toy         |     dog-only data
   +---------------------------+
```

We keep code examples to the simpler "function pointers in a struct" form; just
know embedding is the next step, and it's how real frameworks scale this up.

---

## 8. This is literally how C++ works

When you write C++:

```cpp
struct Animal { virtual void speak() = 0; };
struct Dog : Animal { void speak() override { puts("Woof"); } };
Animal *a = new Dog();
a->speak();   // calls Dog::speak
```

the compiler generates, under the hood, *almost exactly* the C code from this
section: each object carries a hidden pointer to a table of function pointers,
and `a->speak()` becomes "look up speak in that table and call it, passing the
object as the hidden `this`." You're not learning a C *workaround* for OOP —
you're learning **what OOP actually is** beneath the syntax. C++ just automates
the wiring. The table-of-pointers refinement is section 14 (vtables).

---

## 9. Trade-offs and pitfalls

| Aspect | Note |
|---|---|
| Flexibility | Huge: open to new types without touching existing code. |
| Cost | One indirect call per method (section 04) + the pointer storage. Usually negligible. |
| `NULL` methods | An object with a `NULL` method pointer crashes when called — initialize all of them (or guard). |
| Forgetting `self` | The #1 bug: `a->speak()` instead of `a->speak(a)`. The method can't find its data without it. |
| Manual wiring | You must set every pointer in the constructor; miss one and it's `NULL`/garbage. |

The "wire every pointer in a constructor" chore is exactly what motivates the
**shared vtable** of the next section: instead of every object storing its own
copies of the method pointers, all dogs share *one* table.

---

## 10. What to take into section 14

- **Polymorphism** = same operation, different behavior per kind of object.
- In C, you put **function pointers inside a struct**; the object **carries its
  own implementation**, and `obj->method(obj, ...)` dispatches to it.
- Every method takes the object as **`self`** (the explicit `this`).
- A set of method pointers is an **interface**; struct embedding fakes
  inheritance.
- This is **what C++ does internally**, not a hack.

Right now, *every object stores its own copy* of all the method pointers. For
many objects of the same type, that's wasteful and error-prone. The fix — one
shared table of function pointers per type — is the **vtable**.

➡️ Continue to **`theory/14-vtables-in-c/`**, after running
**`code/13-polymorphism-in-c/`**.

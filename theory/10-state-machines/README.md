# 10 — State Machines

> So far a function pointer has *selected behavior* (which op, which handler).
> Now we use it to represent **a state of being**. In a function-pointer state
> machine, "what function the pointer currently holds" *is* the current state,
> and each state function decides which state comes next. It's an elegant,
> common pattern in parsers, protocols, game AI, and embedded firmware.

---

## 1. What is a state machine?

A **finite state machine (FSM)** is a system that is always in exactly one of a
finite set of **states**, and moves ("transitions") between them in response to
**inputs/events**. The classic toy example is a turnstile:

```text
   STATES: LOCKED, UNLOCKED
   INPUTS: coin, push

   LOCKED   + coin  -> UNLOCKED   (paid; let them through next push)
   LOCKED   + push  -> LOCKED     (no pay; stays locked)
   UNLOCKED + push  -> LOCKED     (they went through; lock again)
   UNLOCKED + coin  -> UNLOCKED   (already paid; ignore extra coin)
```

```text
            coin
       +------------->+
   [LOCKED]        [UNLOCKED]
       ^<-------------+
            push
```

You've surely written these with an `enum` and a `switch`. That works, but as
states multiply, the `switch` becomes a sprawling beast. Function pointers offer
a clean alternative.

---

## 2. The classic `switch` style (the baseline)

```c
typedef enum { LOCKED, UNLOCKED } State;

State next(State s, const char *input) {
    switch (s) {
        case LOCKED:
            if (strcmp(input, "coin") == 0) return UNLOCKED;
            return LOCKED;
        case UNLOCKED:
            if (strcmp(input, "push") == 0) return LOCKED;
            return UNLOCKED;
    }
    return s;
}
```

Fine for two states. But with 10 states and 10 inputs, you get a giant nested
`switch`, and adding a state means editing one massive function. Let's see the
function-pointer alternative.

---

## 3. The function-pointer style: a state *is* a function

Key idea:

> Represent each state as a **function**. The "current state" is a **function
> pointer** holding that state's function. Each state function does the state's
> work and **returns the next state's function pointer.**

The state pointer's *type* is a function pointer to "a state function," and a
state function returns the same type — a function pointer to itself-or-another.
That's slightly self-referential, so we need a `typedef` and a small trick.

```c
/* A state function takes an input and returns the NEXT state function. */
typedef struct state_fn (*StateFn)(const char *input);

/* We can't have a function type return its own pointer type directly in C
   (it'd be infinitely recursive in the type), so we wrap it in a struct. */
struct state_fn { StateFn fn; };
```

Don't be alarmed by the wrapper struct — it exists purely to break C's
"a function type can't directly mention its own pointer type as its return type"
limitation. The *idea* is simply: **each state returns the next state.**

A cleaner, more common formulation uses an explicit transition signature where
the machine holds the current state pointer and each handler sets the next:

```c
typedef void (*StateFn)(struct machine *m, const char *input);

struct machine {
    StateFn current;     // the current state IS this function pointer
};
```

Each state function inspects the input and updates `m->current` to the next
state. We'll use this clean version in code; it avoids the recursive-type
gymnastics and reads naturally.

---

## 4. The turnstile as function-pointer states

```c
struct machine;
typedef void (*StateFn)(struct machine *m, const char *input);

struct machine {
    StateFn current;
    const char *name;     // just for printing
};

/* forward declarations so the states can reference each other */
void state_locked(struct machine *m, const char *input);
void state_unlocked(struct machine *m, const char *input);

void state_locked(struct machine *m, const char *input) {
    if (strcmp(input, "coin") == 0) {
        m->current = state_unlocked;   // TRANSITION: now in UNLOCKED
        m->name = "UNLOCKED";
    }
    // else: stay locked (do nothing)
}

void state_unlocked(struct machine *m, const char *input) {
    if (strcmp(input, "push") == 0) {
        m->current = state_locked;     // TRANSITION: back to LOCKED
        m->name = "LOCKED";
    }
}
```

Driving it:

```c
struct machine m = { state_locked, "LOCKED" };
m.current(&m, "push");   // locked + push -> still locked
m.current(&m, "coin");   // locked + coin -> unlocked
m.current(&m, "push");   // unlocked + push -> locked
```

```text
   m.current ----> state_locked        (the pointer IS the state)
        |
        | feed "coin"; state_locked sets m.current = state_unlocked
        v
   m.current ----> state_unlocked      (transition = reassigning the pointer)
```

**A transition is just `m->current = some_other_state;`** — reassigning a
function pointer, exactly section 03. The "current state" and "current behavior"
are literally the same variable.

---

## 5. Why this is nicer than a big switch

- **Each state is self-contained.** All of `state_locked`'s logic lives in one
  small function. To understand a state, read one function — not one branch of a
  monster `switch`.
- **Adding a state = adding a function**, then wiring transitions into it. You
  don't edit a giant central dispatcher.
- **Transitions are explicit and local.** `m->current = state_x;` says exactly
  where you're going, right where the decision is made.
- **The machine's "position" is a single pointer** you can store, log, compare,
  or even save/restore.

The `switch` version centralizes everything (sometimes good for a tiny FSM); the
function-pointer version distributes logic per-state (scales better, reads
better for non-trivial machines). Both are valid tools; know when each fits.

```text
   SWITCH STYLE                         FUNCTION-POINTER STYLE
   ------------                         ----------------------
   one big next() with nested switch    one small function per state
   add state -> edit the big function   add state -> add a function
   state = an enum value                state = a function pointer
   transition = return ENUM             transition = current = state_fn
```

---

## 6. Entry/exit actions and richer states

Real state machines often do work *on entering* or *while in* a state, not just
on transition. Because a state is a function, this is natural — the function can
do anything: print, start a timer, send a message, then decide the next state.

You can also extend each state to a *struct* bundling a function pointer plus
metadata (name, allowed inputs, an on-enter callback). That's a short step from
section 13's "object with behavior":

```c
typedef struct {
    const char *name;
    void (*on_enter)(void);
    StateFn    handle;            // function pointer for input handling
} State;
```

Now a state carries both data and behavior — you're inching toward objects, which
is exactly where sections 13–14 go.

---

## 7. Where state machines (and this pattern) show up

- **Parsers / lexers:** "in-string state," "in-number state," "in-comment
  state." Each character drives a transition.
- **Network protocols:** TCP itself is a state machine (LISTEN, SYN_SENT,
  ESTABLISHED, ...). Protocol libraries often implement it with per-state
  handlers.
- **Game AI:** an enemy is in PATROL, CHASE, ATTACK, FLEE; each tick the current
  state function runs and may transition.
- **Embedded firmware:** device modes (IDLE, MEASURING, TRANSMITTING,
  SLEEPING), very commonly done with function-pointer states to save code space
  and stay readable.
- **UI flows / wizards:** step 1 → step 2 → confirm, each a state.

When you see "the system behaves totally differently depending on what mode it's
in, and modes change in response to events," think state machine — and consider
function-pointer states.

---

## 8. A subtle but important point: the pointer carries the behavior

In every prior section, a function pointer chose behavior for *one call*. In a
state machine, the pointer **persists** — it's the system's memory of "who I am
right now." Calling `m->current(...)` repeatedly does different things over time,
not because the call site changed, but because the *state pointer evolved*. This
"the pointer is durable program state" idea is a real conceptual step up, and it
generalizes directly to objects holding method pointers (next sections).

---

## 9. Pitfalls

| Pitfall | Consequence | Fix |
|---|---|---|
| Forgetting to set the next state | Stuck in one state forever | Ensure every relevant input path sets `current` |
| `current` left `NULL` | Crash on next call | Initialize it; never transition to `NULL` unless "halt" is intended |
| Infinite transition loops | Hang | Make sure inputs eventually reach a stable/terminal state |
| Shared mutable data across states | Subtle bugs | Keep state-specific data in the machine struct, transition cleanly |

A common, *deliberate* use of `NULL` here: a `NULL` `current` means "machine
halted/terminal." Just be intentional about it and guard before calling.

---

## 10. What to take into section 11

- A **state machine** is "one of N states at a time, transitioning on input."
- In the function-pointer style, **a state is a function**, the **current state
  is a function pointer**, and a **transition is reassigning that pointer.**
- It distributes logic per-state (scales better than a giant `switch`) and makes
  the machine's position a single, storable pointer.
- The state pointer is **durable program state** — a step toward objects.
- It powers parsers, protocols, game AI, and firmware.

We've now used single pointers, arrays of pointers (briefly), and persistent
pointers. Next we formalize the **array-of-function-pointers** idea into the
**dispatch table** — the clean replacement for big `switch` statements.

➡️ Continue to **`theory/11-dispatch-tables/`**, after running
**`code/10-state-machines/`**.

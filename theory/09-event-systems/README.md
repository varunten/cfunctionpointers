# 09 — Event Systems

> A callback is *one* function someone calls back. An **event system** is the
> grown-up version: many functions ("listeners") registered to be called when
> something ("an event") happens. This is how GUIs, game engines, web
> frameworks, and operating systems are wired. It's all function pointers in a
> list.

---

## 1. The mental model: "tell me when X happens"

Real programs are reactive. A button gets clicked, a key is pressed, a packet
arrives, a timer fires, a file changes. You don't know *when* these happen, so
you can't write straight-line code for them. Instead you say:

> "Here's a function. **Call it when the button is clicked.**"

That function is an **event handler** (or *listener*, or *observer*). The button
is an **event source**. The act of saying "call this when..." is
**registration** (or *subscribing*). When the event happens, the source
**dispatches** (or *fires*, *emits*) — it calls all the registered handlers.

```text
   You:    "when CLICK happens, call my_handler"   (register)
   Later:  *click*                                  (event occurs)
   System: calls my_handler()                       (dispatch)
```

This is section 07's inversion of control, scaled up: instead of handing one
callback to one function, you register many callbacks against many event types,
and the system calls the right ones at the right time.

---

## 2. The button-click analogy (the canonical example)

Every GUI tutorial starts here, and for good reason. A button doesn't know what
*your* program should do when clicked — that's app-specific. So the button just
stores a function pointer and calls it on click:

```c
typedef void (*ClickHandler)(void);

struct button {
    const char  *label;
    ClickHandler on_click;   // the function to call when clicked
};

void button_click(struct button *b) {     // called by the GUI when a real click occurs
    if (b->on_click)
        b->on_click();                     // dispatch to your handler
}
```

Your app supplies the behavior:

```c
void save_document(void) { printf("Saving...\n"); }

struct button save_btn = { "Save", save_document };
button_click(&save_btn);   // simulate a click -> prints "Saving..."
```

```text
   struct button "Save"                   YOUR CODE
   +----------------------+               +------------------+
   | label   = "Save"     |               | save_document()  |
   | on_click = ----------|-------------->| { Saving... }    |
   +----------------------+               +------------------+
        |
        | button_click() fires it when clicked
        v
   on_click()  -> indirect call -> save_document runs
```

The button's code was written once by the toolkit author. *What* a click does is
your function pointer. Different buttons → different handlers → different
behavior, same button machinery.

---

## 3. Many listeners: the observer pattern

Often *several* parts of your program care about the same event. Click "Save"
and you might want to: write the file, update the title bar, log the action, and
flash a notification. So an event source keeps a **list of handlers** and calls
them all:

```c
#define MAX_LISTENERS 16

typedef void (*Listener)(void);

struct event {
    Listener listeners[MAX_LISTENERS];
    int count;
};

void on(struct event *e, Listener fn) {     // register / subscribe
    if (e->count < MAX_LISTENERS)
        e->listeners[e->count++] = fn;
}

void emit(struct event *e) {                // dispatch / fire
    for (int i = 0; i < e->count; i++)
        e->listeners[i]();                  // call every registered handler
}
```

```text
   event "save"
   +-------------------------------------------+
   | listeners[0] -> write_file               |
   | listeners[1] -> update_title             |
   | listeners[2] -> log_action               |
   | count = 3                                 |
   +-------------------------------------------+
        |
        | emit(&save_event)  -> calls each in turn
        v
   write_file(); update_title(); log_action();
```

This is the **observer pattern**: many observers subscribe; one `emit` notifies
them all. It's an array of function pointers plus "call them in a loop" — nothing
you haven't already seen, just organized.

---

## 4. Events carry data: the event payload

A bare "it happened" is rarely enough. A click has coordinates; a keypress has a
key code; a network event has bytes. So handlers usually take an **event
argument** describing *what* happened:

```c
typedef struct {
    int x, y;
    int button;     // which mouse button
} ClickEvent;

typedef void (*ClickListener)(const ClickEvent *ev);

void emit_click(struct event *e, const ClickEvent *ev) {
    for (int i = 0; i < e->count; i++)
        e->listeners[i](ev);    // pass the event details to each handler
}
```

And, as in section 07, handlers often also need their *own* context, so mature
event systems pass a `void *user_data` alongside the event:

```c
typedef void (*Listener)(const ClickEvent *ev, void *user_data);
```

The event tells the handler *what happened*; the `user_data` tells it *on whose
behalf*. Together they make a handler fully general.

---

## 5. Step-by-step: the full lifecycle

```text
   1. SETUP: create an event source (e.g. the "click" event of a button).

   2. REGISTER: app calls on(&click, my_handler).
        -> my_handler's ADDRESS is stored in the listeners array.

   3. WAIT: program runs its main loop, doing other things.

   4. EVENT OCCURS: the user clicks. The system builds a ClickEvent {x,y,...}.

   5. DISPATCH: the system calls emit_click(&click, &event).
        -> loops the listeners array
        -> for each: indirect call into the app's handler, passing the event

   6. HANDLE: each handler runs its app-specific behavior.

   7. RETURN: dispatch finishes; back to the main loop; wait for the next event.
```

Steps 2 and 5 are the function-pointer heart: **store an address now, call
through it later.** Everything else is bookkeeping (a list, a loop, an event
struct).

---

## 6. The event loop (where dispatch comes from)

Who calls `emit`? In real systems, a central **event loop** sits at the heart of
the program, pulling events from a queue and dispatching each one:

```c
while (running) {
    Event ev = wait_for_next_event();   // blocks until something happens
    dispatch(ev);                        // calls the handlers registered for ev.type
}
```

This is literally the core of GUI frameworks (Win32's message loop, GTK's
`g_main_loop`, the browser's event loop), game engines, and `select`/`epoll`/
`libuv`-style network servers. The whole architecture is: *a loop that turns
incoming events into function-pointer calls.* If you understand the small
version in this section, you understand the shape of all of them.

---

## 7. Registering and unregistering

A robust event system also lets you **remove** a handler ("stop telling me about
clicks"). The simplest approach: find the matching function pointer in the array
and remove it.

```c
void off(struct event *e, Listener fn) {
    for (int i = 0; i < e->count; i++) {
        if (e->listeners[i] == fn) {           // function pointers compare by address
            e->listeners[i] = e->listeners[--e->count];  // swap with last, shrink
            return;
        }
    }
}
```

Note you can compare function pointers with `==` — it compares the **addresses**,
so "is this the same handler I registered?" just works. Production systems often
use a linked list or a handle/ID instead of an array (so removal is cleaner and
order is preserved), but the principle is identical: a collection of function
pointers you can add to and remove from.

---

## 8. Why design this way?

- **Decoupling:** the button (or network layer, or timer) knows *nothing* about
  your app. It only knows "call my listeners." You can change app behavior
  without touching the event source.
- **Extensibility:** add a new feature by registering a new listener — no
  changes to the dispatcher.
- **Reusability:** the same event machinery serves clicks, keypresses, timers,
  custom app events — anything.

This is the same payoff as callbacks (section 07) and dispatch tables (section
11), just expressed as "many subscribers to named events." Recognizing it as
*function pointers in a list* strips away the mystique.

---

## 9. Pitfalls (so your event system doesn't bite you)

| Pitfall | Why it hurts | Mitigation |
|---|---|---|
| Calling a `NULL` listener | Crash | Guard, or never store `NULL` |
| Handler outlives its data | `user_data` dangles when fired later (async!) | Ensure data lives as long as the registration |
| Modifying the listener list *during* dispatch | A handler that unsubscribes mid-loop can corrupt iteration | Iterate a snapshot, or defer removals |
| Unbounded listeners | Fixed array overflows | Bounds-check, or use a dynamic list |
| Re-entrancy (emit inside a handler) | Surprising recursion | Document; sometimes queue instead of calling directly |

The dangling-`user_data` one is the async-lifetime warning from section 07,
which matters even more here because dispatch happens "later."

---

## 10. What to take into section 10

- An **event system** = register many handlers, **dispatch** (call them) when an
  event fires. Inversion of control, scaled up.
- It's built from **arrays/lists of function pointers** plus a loop.
- Events carry a **payload** (what happened) and usually a **`void *user_data`**
  (for whom).
- A central **event loop** turns incoming events into function-pointer calls —
  the backbone of GUIs, servers, and game engines.
- Watch for **NULL**, **lifetime**, and **list-mutation-during-dispatch** bugs.

Next: function pointers that represent *states*, where the "current behavior"
itself decides what behavior comes next — **state machines**.

➡️ Continue to **`theory/10-state-machines/`**, after running
**`code/09-event-systems/`**.

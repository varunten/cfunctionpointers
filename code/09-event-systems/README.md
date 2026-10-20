# Code — 09 Event Systems

Build an event system from scratch: a single click handler, then many listeners,
then events that carry data, then a tiny event-loop simulation.

## Files

| File | What it shows |
|------|----------------|
| `01_button_click.c` | A button stores one `on_click` handler and fires it. |
| `02_many_listeners.c` | Register multiple listeners; `emit` calls them all (observer pattern). |
| `03_event_payload.c` | Events carry data (`ClickEvent`) plus `void *user_data`. |
| `04_event_loop.c` | A mini event loop pulls events from a queue and dispatches by type. |

## Build & run

```sh
gcc 01_button_click.c   -o btn   && ./btn
gcc 02_many_listeners.c -o many  && ./many
gcc 03_event_payload.c  -o payload && ./payload
gcc 04_event_loop.c     -o loop  && ./loop
```

## What to look for

- In `02`, adding a feature means *registering another function* — the
  dispatcher never changes. That's the extensibility win.
- In `04`, the loop is the heart of every GUI/server/game engine: take an event,
  turn it into function-pointer calls, repeat.

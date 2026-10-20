# Code — 10 State Machines

Build state machines where the current state *is* a function pointer and a
transition is reassigning it.

## Files

| File | What it shows |
|------|----------------|
| `01_turnstile_switch.c` | Baseline: the turnstile FSM done with an `enum` + `switch`. |
| `02_turnstile_fnptr.c` | Same turnstile, but each state is a function and `current` is a function pointer. |
| `03_traffic_light.c` | A 3-state cycle (GREEN→YELLOW→RED) driven by ticks; states transition themselves. |

## Build & run

```sh
gcc 01_turnstile_switch.c -o tsw   && ./tsw
gcc 02_turnstile_fnptr.c  -o tfp   && ./tfp
gcc 03_traffic_light.c    -o light && ./light
```

## What to look for

- Compare `01` and `02` directly. Same behavior; one centralizes logic in a
  `switch`, the other distributes it across per-state functions. Notice the
  transition in `02` is literally `m->current = state_x;`.
- In `03`, no input is needed — each "tick" the current state runs and points
  the machine at the next state, cycling forever.

# Code — 18 Capstone Projects

Seven complete, runnable programs that combine the whole series. Read
`theory/18-capstone-projects/` for the step-by-step build plan and pattern maps;
here are the reference implementations.

## Files

| File | Project | Main patterns |
|------|---------|---------------|
| `01_command_dispatcher.c` | Mini shell / command dispatcher | dispatch table (11), callbacks (07) |
| `02_calculator.c` | Calculator via dispatch tables | dispatch/jump tables (11,12) |
| `03_event_system.c` | Reusable event bus / observer | events (09), register API (15) |
| `04_stack_vm.c` | Bytecode stack VM with jumps | jump tables (12), state (10) |
| `05_plugin_system.c` | Versioned plugin host + registry | plugins (16), ops structs (14,15) |
| `06_game_state_machine.c` | Text game state machine | state machines (10) |
| `07_object_system.c` | Shapes with a shared vtable | vtables (14), polymorphism (13) |

## Build & run

```sh
gcc 01_command_dispatcher.c   -o dispatcher && ./dispatcher
gcc 02_calculator.c           -o calc       && ./calc
gcc 03_event_system.c         -o eventbus   && ./eventbus
gcc 04_stack_vm.c             -o vm2        && ./vm2
gcc 05_plugin_system.c        -o plugins    && ./plugins
gcc 06_game_state_machine.c   -o game       && ./game
gcc 07_object_system.c        -o objects    && ./objects -lm
```

(Project 7 uses `sqrt`/`M_PI`; link with `-lm` if your platform needs it.)

Most projects run a scripted demo so they need no interactive input. Where a real
program would read from the user, the demo feeds a fixed sequence and prints what
happens, so you can read the flow top to bottom.

## How to study these

1. Pick a project; read its plan in the theory README first.
2. Try to build it yourself from the incremental steps.
3. Then diff your mental model against this reference.
4. Do a stretch goal — that's where it sticks.

Every one of these is, structurally, the kind of code section 17 taught you to
read. You're now writing it.

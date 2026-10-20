# instructions.md

# Goal

Create an extremely detailed educational markdown series that teaches function pointers in C from first principles.

The learner:

* knows basic C
* understands variables, pointers, arrays, structs
* has built linked lists, stacks, queues, hashmaps
* can write normal functions
* can use normal pointers
* becomes confused when function pointers appear
* does not understand:

  * where functions live in memory
  * what a function address actually is
  * why function pointers exist
  * how callbacks work
  * how APIs use function pointers
  * how polymorphism is implemented in C
  * how virtual dispatch is emulated in C
  * how operating systems, libraries, and frameworks use function pointers

The goal is not merely to learn syntax.

The goal is to understand:

1. what function pointers actually are
2. how they work at the memory level
3. how CPUs use them
4. how indirect calls work
5. how callbacks work
6. how APIs are designed around them
7. how plugin systems work
8. how event systems work
9. how object-oriented behavior can be implemented in C
10. how large systems use function pointers

The learner should finish the series able to read and design APIs using function pointers confidently.

---

# Output Structure

Create this exact structure:

```text
project-root/
|
├── theory/
│   ├── 01-functions-in-memory/
│   ├── 02-function-addresses/
│   ├── 03-basic-function-pointers/
│   ├── 04-calling-through-pointers/
│   ├── 05-function-pointer-syntax/
│   ├── 06-typedefs/
│   ├── 07-callbacks/
│   ├── 08-comparators-and-sorting/
│   ├── 09-event-systems/
│   ├── 10-state-machines/
│   ├── 11-dispatch-tables/
│   ├── 12-jump-tables/
│   ├── 13-polymorphism-in-c/
│   ├── 14-vtables-in-c/
│   ├── 15-api-design/
│   ├── 16-plugin-architectures/
│   ├── 17-reading-real-code/
│   ├── 18-capstone-projects/
│
├── code/
│   ├── 01-functions-in-memory/
│   ├── 02-function-addresses/
│   ├── 03-basic-function-pointers/
│   ├── 04-calling-through-pointers/
│   ├── 05-function-pointer-syntax/
│   ├── 06-typedefs/
│   ├── 07-callbacks/
│   ├── 08-comparators-and-sorting/
│   ├── 09-event-systems/
│   ├── 10-state-machines/
│   ├── 11-dispatch-tables/
│   ├── 12-jump-tables/
│   ├── 13-polymorphism-in-c/
│   ├── 14-vtables-in-c/
│   ├── 15-api-design/
│   ├── 16-plugin-architectures/
│   ├── 17-reading-real-code/
│   ├── 18-capstone-projects/
```

---

# Most Important Rule

This series is NOT about memorizing syntax.

Most learners struggle because they memorize declarations like:

```c
int (*func)(int, int);
```

without understanding what is happening.

This series must focus on mental models first.

Syntax comes second.

---

# Teaching Style

Every topic must answer:

* What problem does this solve?
* Why does it exist?
* What happens in memory?
* What does the CPU do?
* What does the generated machine code conceptually do?
* Where is the function stored?
* What address is being stored?
* What is being passed around?

Always connect explanations back to:

* variables
* pointers
* memory addresses

Never assume knowledge.

---

# ASCII Diagram Requirement

Use diagrams constantly.

Example:

```text
Program Memory

+----------------------+
| Machine Instructions |
| add()                |
+----------------------+
| Machine Instructions |
| multiply()           |
+----------------------+
```

Function address:

```text
Function

add()

Address:
0x401000
```

Function pointer:

```text
fp
 |
 v
0x401000
```

Call:

```text
fp()

↓

Jump to

0x401000

↓

Execute add()
```

Visualize everything.

---

# 01 — Functions in Memory

This section must be extremely detailed.

Teach:

* what a function really is
* machine instructions
* executable code
* text/code segment
* program memory layout

Explain:

Functions are not variables.

Functions are compiled instructions stored somewhere in memory.

Show memory diagrams.

Discuss:

* stack
* heap
* data segment
* text segment

Explain where functions live.

---

# 02 — Function Addresses

Teach:

Every function has an address.

Examples:

```c
add
```

and

```c
&add
```

Explain why they are effectively the same in many contexts.

Show addresses visually.

Explain:

The address points to the first machine instruction.

---

# 03 — Basic Function Pointers

Teach:

What is a function pointer?

Compare:

```c
int *ptr;
```

vs

```c
int (*func_ptr)(int, int);
```

Explain similarities and differences.

Show memory diagrams.

---

# 04 — Calling Through Pointers

Teach:

```c
func_ptr()
```

Explain:

Indirect function calls.

Visualize:

Pointer

↓

Address

↓

CPU jump

↓

Function execution

Discuss:

Direct call vs indirect call.

---

# 05 — Function Pointer Syntax

This section must be extremely detailed.

Most beginners hate this syntax.

Break declarations into pieces.

Explain:

```c
int (*fp)(int, int);
```

symbol by symbol.

Create many examples.

Build confidence gradually.

---

# 06 — Typedefs

Teach:

Why typedefs exist.

Compare:

```c
int (*callback)(int);
```

with:

```c
typedef int (*Callback)(int);
```

Explain readability improvements.

Show API examples.

---

# 07 — Callbacks

This is the first major application.

Teach:

* inversion of control
* user-supplied behavior
* callback design

Visualize:

```text
Library
   |
   v
User Function
```

Explain why callbacks are powerful.

Build multiple examples.

---

# 08 — Comparators and Sorting

Teach:

Why qsort uses callbacks.

Explain:

```c
int compare(const void *, const void *);
```

Walk through sorting.

Visualize comparator calls.

Show how behavior is injected.

---

# 09 — Event Systems

Teach:

* events
* listeners
* handlers
* registration

Build:

* button click analogy
* event dispatch system

Visualize event flow.

---

# 10 — State Machines

Teach:

Using function pointers as states.

Show:

```text
State A
   |
   v
State B
```

implemented via function pointers.

Build small examples.

---

# 11 — Dispatch Tables

Teach:

Replacing giant switch statements.

Visualize:

```text
Index

0 -> add
1 -> subtract
2 -> multiply
```

Explain lookup and execution.

---

# 12 — Jump Tables

Teach:

How interpreters and parsers often dispatch behavior.

Discuss:

* opcode execution
* command execution

Show table-based execution.

---

# 13 — Polymorphism in C

Teach:

How C emulates object-oriented behavior.

Explain:

Common interface.

Different implementations.

Use function pointers.

Examples:

```text
Animal

Dog
Cat
Bird
```

without C++.

---

# 14 — VTables in C

This section must be very detailed.

Teach:

* virtual dispatch
* vtable concept
* interface tables

Visualize:

```text
Object
 |
 v
VTable
 |
 +--> speak
 |
 +--> move
```

Explain how C++ virtual functions conceptually work.

Then implement similar behavior manually in C.

---

# 15 — API Design

Teach:

How libraries expose callbacks.

Examples:

* logging libraries
* networking libraries
* GUI libraries

Explain design tradeoffs.

Discuss ownership and lifetime.

---

# 16 — Plugin Architectures

Teach:

How plugins communicate through function tables.

Visualize:

```text
Host
 |
 +--> Function Table
 |
 Plugin
```

Build understanding of extensibility.

---

# 17 — Reading Real Code

Teach learners how to read function-pointer-heavy codebases.

Walk through patterns found in:

* qsort
* pthreads
* networking libraries
* embedded systems
* operating systems

Focus on recognizing patterns.

---

# 18 — Capstone Projects

Create projects that heavily use function pointers.

Examples:

* command dispatcher
* event system
* calculator using dispatch tables
* plugin system
* mini game state machine
* mini object system
* mini virtual table implementation

Build step-by-step.

---

# Code Folder Requirements

Code should be incremental.

Never dump large programs immediately.

Example progression:

1. store function address
2. call through pointer
3. pass function pointer
4. return function pointer
5. build callback system
6. build dispatch table
7. build event system

Each file should explain:

* what memory changes
* what address is stored
* what call occurs

---

# Reading and Visualization Requirements

Every major concept must contain:

1. source code
2. memory diagram
3. execution diagram
4. pointer diagram
5. step-by-step walkthrough

The learner should be able to visualize what happens internally.

---

# Final Objective

By the end of the series, the learner should:

* understand where functions live in memory
* understand function addresses
* understand indirect calls
* understand callbacks
* understand dispatch tables
* understand event systems
* understand API design using callbacks
* understand plugin architectures
* understand polymorphism in C
* understand virtual dispatch concepts
* read function-pointer-heavy code confidently
* design their own APIs using function pointers

The learner should stop seeing function pointers as strange syntax and instead see them as ordinary pointers that happen to point to executable code.

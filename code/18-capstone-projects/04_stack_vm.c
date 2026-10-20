/* 04_stack_vm.c  -- Capstone Project 4: a bytecode stack VM with branches.
 *
 * Extends section 12's VM with DUP and conditional/unconditional jumps, making
 * it able to LOOP. Patterns: jump table (12), program counter / state (10),
 * op_illegal safety (12).
 *
 * The demo program counts down from 5, printing each value, using a JZ branch.
 *
 * Build: gcc 04_stack_vm.c -o vm2 && ./vm2
 */
#include <stdio.h>
#include <stdint.h>

typedef enum {
    OP_PUSH,   /* PUSH n   : push immediate n            */
    OP_DUP,    /* DUP      : duplicate top               */
    OP_SUB,    /* SUB      : b=pop,a=pop, push a-b        */
    OP_PRINT,  /* PRINT    : print top (without popping)  */
    OP_JZ,     /* JZ addr  : pop; if 0, pc=addr           */
    OP_JMP,    /* JMP addr : pc=addr                      */
    OP_HALT,
    OP_COUNT
} OpCode;

typedef struct {
    const uint8_t *code;
    int            pc;
    int            stack[64];
    int            sp;
    int            running;
} VM;

static void push(VM *v, int x) { v->stack[v->sp++] = x; }
static int  pop(VM *v)         { return v->stack[--v->sp]; }
static int  top(VM *v)         { return v->stack[v->sp - 1]; }

typedef void (*OpHandler)(VM *);

static void h_push(VM *v)  { push(v, v->code[v->pc++]); }
static void h_dup(VM *v)   { push(v, top(v)); }
static void h_sub(VM *v)   { int b = pop(v), a = pop(v); push(v, a - b); }
static void h_print(VM *v) { printf("%d ", top(v)); }
static void h_jz(VM *v)    { int addr = v->code[v->pc++]; if (pop(v) == 0) v->pc = addr; }
static void h_jmp(VM *v)   { int addr = v->code[v->pc++]; v->pc = addr; }
static void h_halt(VM *v)  { v->running = 0; }
static void h_illegal(VM *v){ printf("\n[illegal opcode -> halt]\n"); v->running = 0; }

int main(void) {
    OpHandler ops[256];
    for (int i = 0; i < 256; i++) ops[i] = h_illegal;   /* safe default */
    ops[OP_PUSH]=h_push; ops[OP_DUP]=h_dup; ops[OP_SUB]=h_sub;
    ops[OP_PRINT]=h_print; ops[OP_JZ]=h_jz; ops[OP_JMP]=h_jmp; ops[OP_HALT]=h_halt;

    /*
     * Countdown from 5 to 1:
     *   push 5
     * loop: (addr 2)
     *   dup; print           ; show current value (keeps it on the stack)
     *   push 1; sub          ; n = n - 1
     *   dup; jz end          ; if n==0 goto end (jz pops the test copy)
     *   jmp loop
     * end: halt
     *
     * Byte addresses are written as comments so the jump targets line up.
     */
    uint8_t program[] = {
        /* 0 */ OP_PUSH, 5,
        /* 2 */ OP_DUP,
        /* 3 */ OP_PRINT,
        /* 4 */ OP_PUSH, 1,
        /* 6 */ OP_SUB,
        /* 7 */ OP_DUP,
        /* 8 */ OP_JZ, 13,        /* if n==0 -> halt at addr 13 */
        /*10 */ OP_JMP, 2,        /* else loop back to addr 2 */
        /*12 */ OP_HALT,          /* (unused fall-through guard) */
        /*13 */ OP_HALT,
    };

    VM vm = { .code = program, .pc = 0, .sp = 0, .running = 1 };

    printf("countdown: ");
    while (vm.running) {
        uint8_t op = vm.code[vm.pc++];   /* FETCH */
        ops[op](&vm);                    /* DECODE + EXECUTE via jump table */
    }
    printf("\n\nThis VM loops using a program counter and JZ/JMP -- a real CPU\n");
    printf("in miniature, dispatched entirely through a jump table.\n");
    return 0;
}

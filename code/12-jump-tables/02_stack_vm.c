/* 02_stack_vm.c
 *
 * A complete tiny STACK VM driven by a jump table. It has:
 *   - a bytecode array (the program)
 *   - a program counter (pc) walking it
 *   - operands (PUSH reads the next byte as its value)
 *   - a stack the handlers operate on
 *   - a running flag that HALT clears
 *
 * It computes (5 + 3) * 2 and prints 16.
 *
 * Build: gcc 02_stack_vm.c -o vm && ./vm
 */
#include <stdio.h>
#include <stdint.h>

typedef enum { OP_PUSH, OP_ADD, OP_SUB, OP_MUL, OP_PRINT, OP_HALT, OP_COUNT } OpCode;

typedef struct {
    const uint8_t *code;   /* bytecode */
    int            pc;     /* program counter */
    int            stack[64];
    int            sp;     /* stack pointer (next free slot) */
    int            running;
} VM;

static void push(VM *vm, int v) { vm->stack[vm->sp++] = v; }
static int  pop(VM *vm)         { return vm->stack[--vm->sp]; }

typedef void (*OpHandler)(VM *vm);

void op_push(VM *vm)  { int v = vm->code[vm->pc++]; push(vm, v); }   /* operand! */
void op_add(VM *vm)   { int b = pop(vm), a = pop(vm); push(vm, a + b); }
void op_sub(VM *vm)   { int b = pop(vm), a = pop(vm); push(vm, a - b); }
void op_mul(VM *vm)   { int b = pop(vm), a = pop(vm); push(vm, a * b); }
void op_print(VM *vm) { printf("=> %d\n", pop(vm)); }
void op_halt(VM *vm)  { vm->running = 0; }

static OpHandler ops[OP_COUNT] = {
    [OP_PUSH]  = op_push,
    [OP_ADD]   = op_add,
    [OP_SUB]   = op_sub,
    [OP_MUL]   = op_mul,
    [OP_PRINT] = op_print,
    [OP_HALT]  = op_halt,
};

int main(void) {
    /* program: (5 + 3) * 2, then print, then halt */
    uint8_t program[] = {
        OP_PUSH, 5,
        OP_PUSH, 3,
        OP_ADD,
        OP_PUSH, 2,
        OP_MUL,
        OP_PRINT,
        OP_HALT
    };

    VM vm = { .code = program, .pc = 0, .sp = 0, .running = 1 };

    /* The fetch-decode-execute loop -- the heart of every bytecode VM. */
    while (vm.running) {
        uint8_t opcode = vm.code[vm.pc++];   /* FETCH */
        ops[opcode](&vm);                    /* DECODE + EXECUTE */
    }

    printf("\nThe loop dispatched each opcode through the jump table. This is\n");
    printf("structurally how CPython, Lua, and the JVM execute bytecode.\n");
    return 0;
}

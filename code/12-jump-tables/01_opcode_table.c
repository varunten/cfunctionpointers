/* 01_opcode_table.c
 *
 * The minimal jump-table idea: opcodes are small integers; ops[] maps each to a
 * handler function pointer; a loop fetches opcodes and calls handlers. This is
 * the skeleton of every bytecode interpreter.
 *
 * Build: gcc 01_opcode_table.c -o ops && ./ops
 */
#include <stdio.h>

typedef enum { OP_HELLO, OP_BYE, OP_BEEP, OP_COUNT } OpCode;

typedef void (*OpHandler)(void);

void op_hello(void) { printf("hello\n"); }
void op_bye(void)   { printf("goodbye\n"); }
void op_beep(void)  { printf("beep!\n"); }

static OpHandler ops[OP_COUNT] = {
    [OP_HELLO] = op_hello,
    [OP_BYE]   = op_bye,
    [OP_BEEP]  = op_beep,
};

int main(void) {
    /* A "program": a stream of opcodes. */
    OpCode program[] = { OP_HELLO, OP_BEEP, OP_BEEP, OP_BYE };
    int n = (int)(sizeof program / sizeof program[0]);

    for (int pc = 0; pc < n; pc++) {
        OpCode op = program[pc];        /* FETCH */
        ops[op]();                      /* DECODE (index) + EXECUTE (call) */
    }

    printf("\nFetch an opcode, index ops[], call the handler. Add an opcode\n");
    printf("by adding a function and a table row -- the loop never changes.\n");
    return 0;
}

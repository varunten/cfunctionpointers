/* 03_illegal_handler.c
 *
 * Safety pattern for jump tables: pre-fill EVERY slot with a default handler
 * (op_illegal) before installing the real ones. Then an unknown/unimplemented
 * opcode runs op_illegal instead of dereferencing a NULL slot (a crash).
 *
 * Build: gcc 03_illegal_handler.c -o illegal && ./illegal
 */
#include <stdio.h>
#include <stdint.h>

#define TABLE_SIZE 256       /* one slot per possible byte opcode */

typedef void (*OpHandler)(int *running);

void op_nop(int *running)  { (void)running; printf("nop\n"); }
void op_halt(int *running) { printf("halt\n"); *running = 0; }

void op_illegal(int *running) {
    printf("ILLEGAL opcode encountered -- stopping safely\n");
    *running = 0;            /* don't crash; report and stop */
}

int main(void) {
    OpHandler ops[TABLE_SIZE];

    /* 1) fill the WHOLE table with the default handler */
    for (int i = 0; i < TABLE_SIZE; i++)
        ops[i] = op_illegal;

    /* 2) overwrite the opcodes we actually implement */
    ops[0] = op_nop;
    ops[1] = op_halt;
    /* opcode 200 is intentionally left as op_illegal */

    uint8_t program[] = { 0, 0, 200, 1 };   /* nop, nop, <illegal>, halt */
    int n = (int)(sizeof program / sizeof program[0]);

    int running = 1;
    for (int pc = 0; pc < n && running; pc++) {
        uint8_t op = program[pc];
        ops[op](&running);        /* every slot is valid -> no NULL deref */
    }

    printf("\nPre-filling with op_illegal means an unknown opcode is handled\n");
    printf("gracefully instead of crashing on a NULL table slot.\n");
    return 0;
}

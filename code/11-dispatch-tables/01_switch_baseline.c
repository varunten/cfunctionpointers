/* 01_switch_baseline.c
 *
 * The thing we're about to replace: a calculator driven by a switch. Perfectly
 * fine for four cases. Compare with 02_array_dispatch.c.
 *
 * Build: gcc 01_switch_baseline.c -o cswitch && ./cswitch
 */
#include <stdio.h>

int calculate(char op, int a, int b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b ? a / b : 0;
        default:  return 0;
    }
}

int main(void) {
    char ops[] = { '+', '-', '*', '/' };
    for (int i = 0; i < 4; i++)
        printf("12 %c 4 = %d\n", ops[i], calculate(ops[i], 12, 4));
    return 0;
}

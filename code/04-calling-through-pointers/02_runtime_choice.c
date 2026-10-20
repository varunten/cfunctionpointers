/* 02_runtime_choice.c
 *
 * The actual POINT of indirect calls: the call site never changes, but which
 * function runs is decided at RUNTIME. Here the choice comes from user input,
 * so the compiler cannot possibly know it in advance -- it MUST be indirect.
 *
 * Build: gcc 02_runtime_choice.c -o choice && ./choice
 */
#include <stdio.h>

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }

int main(void) {
    int (*op)(int, int);   /* the operation, chosen at runtime */
    int choice;

    printf("Pick an operation: 1=add  2=subtract  3=multiply : ");
    if (scanf("%d", &choice) != 1) {
        printf("no input\n");
        return 1;
    }

    switch (choice) {
        case 1:  op = add;      break;
        case 2:  op = subtract; break;
        case 3:  op = multiply; break;
        default: op = add;      break;
    }

    /* ONE call site. Its behavior depends entirely on what 'op' points to. */
    printf("result of op(10, 4) = %d\n", op(10, 4));

    printf("\nThe line 'op(10,4)' is fixed in the source, yet it ran a\n");
    printf("different function depending on your choice. That's only\n");
    printf("possible because the address lived in a variable.\n");
    return 0;
}

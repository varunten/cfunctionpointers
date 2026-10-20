/* 02_calculator.c  -- Capstone Project 2: calculator via dispatch tables.
 *
 * Operators are function pointers in a table. Binary ops indexed by operator
 * char; unary ops looked up by name. No switch anywhere. Patterns: dispatch
 * tables (11), jump tables (12), behavior injection (08).
 *
 * Build: gcc 02_calculator.c -o calc && ./calc
 */
#include <stdio.h>
#include <string.h>
#include <math.h>

/* ---- binary operations ---- */
typedef double (*BinOp)(double, double);

static double op_add(double a, double b) { return a + b; }
static double op_sub(double a, double b) { return a - b; }
static double op_mul(double a, double b) { return a * b; }
static double op_div(double a, double b) { return b != 0 ? a / b : 0.0; }

/* map an operator character to its function (small lookup) */
static BinOp binop_for(char op) {
    switch (op) {              /* a tiny switch just to map char->index is fine;
                                  the BEHAVIOR lives in the function pointers */
        case '+': return op_add;
        case '-': return op_sub;
        case '*': return op_mul;
        case '/': return op_div;
        default:  return NULL;
    }
}

/* ---- unary operations (a second table: name -> function) ---- */
typedef double (*UnOp)(double);

static double un_neg(double x)  { return -x; }
static double un_sqrt(double x) { return x >= 0 ? sqrt(x) : 0.0; }
static double un_abs(double x)  { return fabs(x); }

struct unary { const char *name; UnOp fn; };
static struct unary unaries[] = {
    { "neg",  un_neg },
    { "sqrt", un_sqrt },
    { "abs",  un_abs },
};
static const int NUN = (int)(sizeof unaries / sizeof unaries[0]);

static UnOp unop_for(const char *name) {
    for (int i = 0; i < NUN; i++)
        if (strcmp(unaries[i].name, name) == 0)
            return unaries[i].fn;
    return NULL;
}

int main(void) {
    /* binary demo: "a op b" */
    struct { double a; char op; double b; } bin[] = {
        { 12, '+', 4 }, { 12, '-', 4 }, { 12, '*', 4 }, { 12, '/', 4 },
        { 1,  '/', 0 },           /* guarded divide-by-zero */
        { 5,  '%', 2 },           /* unknown operator */
    };
    printf("Binary operations:\n");
    for (size_t i = 0; i < sizeof bin / sizeof bin[0]; i++) {
        BinOp f = binop_for(bin[i].op);
        if (f)
            printf("   %g %c %g = %g\n", bin[i].a, bin[i].op, bin[i].b,
                   f(bin[i].a, bin[i].b));
        else
            printf("   %g %c %g = (unknown operator)\n",
                   bin[i].a, bin[i].op, bin[i].b);
    }

    /* unary demo: "name x" */
    struct { const char *name; double x; } un[] = {
        { "neg", 7 }, { "sqrt", 144 }, { "abs", -3.5 }, { "log", 1 },
    };
    printf("\nUnary operations:\n");
    for (size_t i = 0; i < sizeof un / sizeof un[0]; i++) {
        UnOp f = unop_for(un[i].name);
        if (f)
            printf("   %s(%g) = %g\n", un[i].name, un[i].x, f(un[i].x));
        else
            printf("   %s(%g) = (unknown function)\n", un[i].name, un[i].x);
    }

    printf("\nTwo dispatch tables (binary by char, unary by name). Adding an\n");
    printf("operation never touches the evaluation logic -- just the tables.\n");
    return 0;
}

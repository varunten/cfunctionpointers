/* 01_decl_gallery.c
 *
 * A gallery of function-pointer declarations. For each, the comment gives the
 * Right-Left reading. Every pointer is actually assigned and called so the file
 * compiles and the readings are confirmed by output.
 *
 * Build: gcc 01_decl_gallery.c -o gallery && ./gallery
 */
#include <stdio.h>
#include <string.h>
#include <math.h>

int    add(int a, int b)          { return a + b; }
void   say_hi(void)               { printf("   hi\n"); }
double square_root(double x)      { return sqrt(x); }
size_t length(const char *s)      { return strlen(s); }

int main(void) {
    /* "f1 is a pointer to a function taking (int,int) returning int" */
    int (*f1)(int, int) = add;

    /* "f2 is a pointer to a function taking no args, returning nothing" */
    void (*f2)(void) = say_hi;

    /* "f3 is a pointer to a function taking a double, returning a double" */
    double (*f3)(double) = square_root;

    /* "f4 is a pointer to a function taking const char*, returning size_t" */
    size_t (*f4)(const char *) = length;

    printf("f1(2,3)        = %d\n", f1(2, 3));
    printf("f2()           -> "); f2();
    printf("f3(2.0)        = %f\n", f3(2.0));
    printf("f4(\"hello\")    = %zu\n", f4("hello"));

    printf("\nEach declaration was read with the Right-Left rule. The call\n");
    printf("syntax mirrors the declaration -- that's not a coincidence.\n");
    return 0;
}

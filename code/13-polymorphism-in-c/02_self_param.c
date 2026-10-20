/* 02_self_param.c
 *
 * Why methods take 'self': a C function isn't attached to an object, so to read
 * the object's data it must receive the object as a parameter. This is the
 * explicit version of C++'s hidden 'this' / Python's 'self'.
 *
 * Convention: obj->method(obj, args...)
 *
 * Build: gcc 02_self_param.c -o selfp && ./selfp
 */
#include <stdio.h>

struct counter {
    const char *label;
    int         value;
    void (*increment)(struct counter *self, int by);
    void (*show)(struct counter *self);
};

/* Each method uses 'self' to reach THIS counter's data. */
void counter_inc(struct counter *self, int by) {
    self->value += by;        /* without self, which counter would we update? */
}
void counter_show(struct counter *self) {
    printf("%s = %d\n", self->label, self->value);
}

struct counter make_counter(const char *label) {
    struct counter c = { label, 0, counter_inc, counter_show };
    return c;
}

int main(void) {
    struct counter a = make_counter("apples");
    struct counter b = make_counter("bananas");

    a.increment(&a, 3);    /* obj->method(obj, args) */
    a.increment(&a, 2);
    b.increment(&b, 10);

    a.show(&a);            /* apples = 5  */
    b.show(&b);            /* bananas = 10 */

    printf("\n'self' is how each method found the right object's data. Every\n");
    printf("C-style method takes the object as its first parameter.\n");
    return 0;
}

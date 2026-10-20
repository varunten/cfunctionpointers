/* 03_override_inherit.c
 *
 * "Overriding" and "inheriting" at the machine level are just choices about
 * which function pointer goes in which vtable slot:
 *   - override  = put a NEW function in the slot
 *   - inherit   = copy the base's function pointer into the slot
 *
 * Here a "loud dog" overrides speak() but inherits describe() from the base dog.
 *
 * Build: gcc 03_override_inherit.c -o vover && ./vover
 */
#include <stdio.h>

struct animal;
struct animal_vtable {
    void (*speak)(struct animal *self);
    void (*describe)(struct animal *self);
};
struct animal { const struct animal_vtable *vtable; const char *name; };

/* Base dog behavior */
void dog_speak(struct animal *s)    { printf("%s: Woof\n", s->name); }
void dog_describe(struct animal *s) { printf("%s is a dog.\n", s->name); }

static const struct animal_vtable dog_vtable = {
    .speak = dog_speak, .describe = dog_describe
};

/* Loud dog OVERRIDES speak, INHERITS describe */
void loud_dog_speak(struct animal *s) { printf("%s: WOOF WOOF WOOF!!!\n", s->name); }

static const struct animal_vtable loud_dog_vtable = {
    .speak    = loud_dog_speak,   /* override: new function */
    .describe = dog_describe,     /* inherit: reuse the base's pointer */
};

int main(void) {
    struct animal rex  = { &dog_vtable,      "Rex"  };
    struct animal zeus = { &loud_dog_vtable, "Zeus" };

    rex.vtable->speak(&rex);        rex.vtable->describe(&rex);
    zeus.vtable->speak(&zeus);      zeus.vtable->describe(&zeus);

    printf("\nZeus overrode speak (different pointer) but inherited describe\n");
    printf("(same pointer as the base). That's all OOP inheritance is at the\n");
    printf("machine level: which function pointer sits in each table slot.\n");
    return 0;
}

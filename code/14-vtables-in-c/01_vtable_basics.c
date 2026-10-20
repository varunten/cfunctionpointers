/* 01_vtable_basics.c
 *
 * The vtable pattern: methods live in ONE shared table per type; each object
 * holds a single pointer to its type's vtable. Virtual dispatch is the two-hop
 * lookup:  a->vtable->speak(a).  This is how C++ virtual functions work.
 *
 * Build: gcc 01_vtable_basics.c -o vt && ./vt
 */
#include <stdio.h>

struct animal;   /* forward declaration */

/* The vtable type: just the method pointers for a kind of animal. */
struct animal_vtable {
    void (*speak)(struct animal *self);
    void (*move)(struct animal *self);
    int  (*legs)(struct animal *self);
};

/* The object: per-instance data + ONE pointer to the shared vtable. */
struct animal {
    const struct animal_vtable *vtable;
    const char *name;
};

/* ---- Dog implementation ---- */
void dog_speak(struct animal *s){ printf("%s: Woof\n", s->name); }
void dog_move(struct animal *s) { printf("%s runs\n", s->name); }
int  dog_legs(struct animal *s) { (void)s; return 4; }

static const struct animal_vtable dog_vtable = {
    .speak = dog_speak, .move = dog_move, .legs = dog_legs
};

/* ---- Cat implementation ---- */
void cat_speak(struct animal *s){ printf("%s: Meow\n", s->name); }
void cat_move(struct animal *s) { printf("%s prowls\n", s->name); }
int  cat_legs(struct animal *s) { (void)s; return 4; }

static const struct animal_vtable cat_vtable = {
    .speak = cat_speak, .move = cat_move, .legs = cat_legs
};

/* Constructors point the object at the shared vtable (no copying of methods). */
struct animal make_dog(const char *n) { struct animal a = { &dog_vtable, n }; return a; }
struct animal make_cat(const char *n) { struct animal a = { &cat_vtable, n }; return a; }

/* Generic code uses virtual dispatch; it doesn't know dog from cat. */
void describe(struct animal *a) {
    a->vtable->speak(a);                    /* object -> vtable -> method */
    a->vtable->move(a);
    printf("%s has %d legs\n\n", a->name, a->vtable->legs(a));
}

int main(void) {
    struct animal rex   = make_dog("Rex");
    struct animal felix = make_cat("Felix");

    describe(&rex);
    describe(&felix);

    printf("Each call went object -> vtable -> method. Same as C++ virtual\n");
    printf("dispatch: a hidden vptr, a shared vtable, one extra indirection.\n");
    return 0;
}

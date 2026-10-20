/* 03_interface.c
 *
 * A multi-method interface. Every animal promises speak/move/legs; each
 * implements them differently. Code written against 'struct animal *' works for
 * ANY animal -- "program to an interface, not an implementation".
 *
 * Build: gcc 03_interface.c -o iface && ./iface
 */
#include <stdio.h>

/* The interface: the set of operations every animal supports. */
struct animal {
    const char *name;
    void (*speak)(struct animal *self);
    void (*move)(struct animal *self);
    int  (*legs)(struct animal *self);
};

/* Dog implementation */
void dog_speak(struct animal *s) { printf("%s: Woof\n", s->name); }
void dog_move(struct animal *s)  { printf("%s runs\n", s->name); }
int  dog_legs(struct animal *s)  { (void)s; return 4; }

/* Bird implementation */
void bird_speak(struct animal *s){ printf("%s: Tweet\n", s->name); }
void bird_move(struct animal *s) { printf("%s flies\n", s->name); }
int  bird_legs(struct animal *s) { (void)s; return 2; }

struct animal make_dog(const char *n)  { struct animal a = { n, dog_speak, dog_move, dog_legs };  return a; }
struct animal make_bird(const char *n) { struct animal a = { n, bird_speak, bird_move, bird_legs }; return a; }

/* Generic code: works on ANY animal through the interface. */
void describe(struct animal *a) {
    a->speak(a);
    a->move(a);
    printf("%s has %d legs\n\n", a->name, a->legs(a));
}

int main(void) {
    struct animal rex   = make_dog("Rex");
    struct animal tweety = make_bird("Tweety");

    describe(&rex);
    describe(&tweety);

    printf("describe() was written ONCE against the interface and worked for\n");
    printf("both a dog and a bird. New animals need no change to describe().\n");
    return 0;
}

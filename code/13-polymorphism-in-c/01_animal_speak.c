/* 01_animal_speak.c
 *
 * Polymorphism in its simplest C form: each animal carries a function pointer
 * to its OWN speak behavior. The loop calls animal.speak(&animal) for each --
 * no switch, no type tag. Each does the right thing for its kind.
 *
 * Build: gcc 01_animal_speak.c -o animals && ./animals
 */
#include <stdio.h>

struct animal {
    const char *name;
    void (*speak)(struct animal *self);   /* each animal points to its own speak */
};

/* Different implementations of the same operation. */
void dog_speak(struct animal *self)  { printf("%s says Woof\n", self->name); }
void cat_speak(struct animal *self)  { printf("%s says Meow\n", self->name); }
void bird_speak(struct animal *self) { printf("%s says Tweet\n", self->name); }

/* "Constructors" wire up the right behavior. */
struct animal make_dog(const char *n)  { struct animal a = { n, dog_speak };  return a; }
struct animal make_cat(const char *n)  { struct animal a = { n, cat_speak };  return a; }
struct animal make_bird(const char *n) { struct animal a = { n, bird_speak }; return a; }

int main(void) {
    struct animal zoo[] = {
        make_dog("Rex"),
        make_cat("Whiskers"),
        make_bird("Tweety"),
        make_dog("Buddy"),
    };
    int n = (int)(sizeof zoo / sizeof zoo[0]);

    /* This loop knows nothing about dogs vs cats vs birds. */
    for (int i = 0; i < n; i++)
        zoo[i].speak(&zoo[i]);     /* polymorphic call */

    printf("\nNo switch, no type tag. Each object carried its own behavior,\n");
    printf("and the loop just asked each one to speak. That's polymorphism.\n");
    return 0;
}

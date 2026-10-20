/* 02_share_proof.c
 *
 * Proof that the vtable is SHARED: many dogs all point their vtable field at the
 * exact same address. That's the memory win over section 13, where every object
 * stored its own copies of the method pointers.
 *
 * Build: gcc 02_share_proof.c -o vshare && ./vshare
 */
#include <stdio.h>

struct animal;
struct animal_vtable { void (*speak)(struct animal *self); };
struct animal { const struct animal_vtable *vtable; const char *name; };

void dog_speak(struct animal *s) { printf("%s: Woof\n", s->name); }
static const struct animal_vtable dog_vtable = { .speak = dog_speak };

struct animal make_dog(const char *n) { struct animal a = { &dog_vtable, n }; return a; }

int main(void) {
    struct animal dogs[] = {
        make_dog("Rex"), make_dog("Buddy"), make_dog("Max"),
    };
    int n = (int)(sizeof dogs / sizeof dogs[0]);

    printf("dog_vtable lives at: %p\n\n", (const void *)&dog_vtable);
    for (int i = 0; i < n; i++)
        printf("%-6s vtable = %p  (same table!)\n",
               dogs[i].name, (const void *)dogs[i].vtable);

    printf("\nEvery dog points at the ONE shared dog_vtable. Method addresses\n");
    printf("exist once, not once per object. That's why vtables scale.\n");
    return 0;
}

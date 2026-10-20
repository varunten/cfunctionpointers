/* 02_qsort_structs.c
 *
 * Sort an array of structs by different fields. qsort doesn't know what a
 * Person is -- it just moves 'size' bytes around and asks your comparator which
 * element comes first. The comparator casts void* back to Person*.
 *
 * Build: gcc 02_qsort_structs.c -o qstruct && ./qstruct
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[32];
    int  age;
} Person;

/* Order by age, ascending. */
int by_age(const void *a, const void *b) {
    const Person *pa = a;
    const Person *pb = b;
    if (pa->age < pb->age) return -1;
    if (pa->age > pb->age) return  1;
    return 0;
}

/* Order alphabetically by name -- reuse strcmp, which already returns -,0,+. */
int by_name(const void *a, const void *b) {
    const Person *pa = a;
    const Person *pb = b;
    return strcmp(pa->name, pb->name);
}

void print_people(const char *label, Person *p, int n) {
    printf("%s\n", label);
    for (int i = 0; i < n; i++)
        printf("   %-8s age %d\n", p[i].name, p[i].age);
}

int main(void) {
    Person people[] = {
        { "Carol", 30 },
        { "Alice", 25 },
        { "Bob",   40 },
        { "Dave",  25 },
    };
    int n = (int)(sizeof people / sizeof people[0]);

    qsort(people, n, sizeof(Person), by_age);
    print_people("Sorted by age:", people, n);

    qsort(people, n, sizeof(Person), by_name);
    print_people("Sorted by name:", people, n);

    printf("\nThe data was identical both times; only the comparator's idea\n");
    printf("of 'order' changed. That's how one sort routine serves any rule.\n");
    return 0;
}

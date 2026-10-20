/* 07_object_system.c  -- Capstone Project 7: shapes with a shared vtable.
 *
 * A tiny "class system": each shape type has ONE shared vtable (area/name/draw);
 * each object holds a vtable pointer plus its own data. Generic code uses them
 * polymorphically -- no switch, no type tags. Patterns: vtables (14),
 * polymorphism (13), struct embedding (13).
 *
 * Build: gcc 07_object_system.c -o objects && ./objects   (add -lm if needed)
 */
#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

struct shape;   /* forward */

/* The interface: a shared table of methods, one table per shape type. */
struct shape_vtable {
    double      (*area)(const struct shape *self);
    const char *(*name)(const struct shape *self);
    void        (*draw)(const struct shape *self);
};

/* The base "object": every shape starts with a vtable pointer. */
struct shape {
    const struct shape_vtable *vtable;
};

/* ---- Circle: embeds shape as first member (inheritance, section 13) ---- */
struct circle {
    struct shape base;     /* MUST be first: lets (struct shape*)circle work */
    double radius;
};

static double circle_area(const struct shape *s) {
    const struct circle *c = (const struct circle *)s;
    return PI * c->radius * c->radius;
}
static const char *circle_name(const struct shape *s) { (void)s; return "circle"; }
static void circle_draw(const struct shape *s) {
    const struct circle *c = (const struct circle *)s;
    printf("   ( o )  r=%.1f\n", c->radius);
}
static const struct shape_vtable circle_vtable = {
    circle_area, circle_name, circle_draw
};

/* ---- Rectangle ---- */
struct rect {
    struct shape base;
    double w, h;
};

static double rect_area(const struct shape *s) {
    const struct rect *r = (const struct rect *)s;
    return r->w * r->h;
}
static const char *rect_name(const struct shape *s) { (void)s; return "rectangle"; }
static void rect_draw(const struct shape *s) {
    const struct rect *r = (const struct rect *)s;
    printf("   [ %gx%g ]\n", r->w, r->h);
}
static const struct shape_vtable rect_vtable = {
    rect_area, rect_name, rect_draw
};

/* constructors wire each object to its type's SHARED vtable */
static struct circle make_circle(double r) {
    struct circle c; c.base.vtable = &circle_vtable; c.radius = r; return c;
}
static struct rect make_rect(double w, double h) {
    struct rect r; r.base.vtable = &rect_vtable; r.w = w; r.h = h; return r;
}

/* generic code: works on ANY shape through the vtable */
static void describe(const struct shape *s) {
    s->vtable->draw(s);
    printf("   %s, area = %.2f\n\n", s->vtable->name(s), s->vtable->area(s));
}

int main(void) {
    struct circle c1 = make_circle(2.0);
    struct circle c2 = make_circle(5.0);
    struct rect   r1 = make_rect(3.0, 4.0);

    /* an array of shape pointers, treated uniformly */
    const struct shape *shapes[] = {
        &c1.base, &r1.base, &c2.base,
    };
    int n = (int)(sizeof shapes / sizeof shapes[0]);

    double total = 0;
    for (int i = 0; i < n; i++) {
        describe(shapes[i]);              /* virtual dispatch */
        total += shapes[i]->vtable->area(shapes[i]);
    }
    printf("total area of all shapes = %.2f\n", total);

    /* proof of sharing: both circles use the same vtable */
    printf("\nc1.vtable = %p\nc2.vtable = %p  (same shared circle vtable)\n",
           (const void *)c1.base.vtable, (const void *)c2.base.vtable);

    printf("\nAdding a Triangle would need NO change to describe() or the loop.\n");
    printf("That's polymorphism via a shared vtable -- exactly how C++ works.\n");
    return 0;
}

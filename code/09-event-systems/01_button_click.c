/* 01_button_click.c
 *
 * The canonical event example: a button stores a function pointer (its click
 * handler) and calls it when "clicked". The button code knows nothing about
 * what your app does -- that's your handler.
 *
 * Build: gcc 01_button_click.c -o btn && ./btn
 */
#include <stdio.h>

typedef void (*ClickHandler)(void);

struct button {
    const char  *label;
    ClickHandler on_click;     /* the function to call when clicked */
};

/* The toolkit calls this when a real click occurs. */
void button_click(struct button *b) {
    printf("[%s] clicked -> ", b->label);
    if (b->on_click)
        b->on_click();          /* dispatch to the app's handler */
    else
        printf("(no handler)\n");
}

/* App-specific behaviors. */
void save_document(void) { printf("saving document...\n"); }
void quit_app(void)      { printf("quitting application...\n"); }

int main(void) {
    struct button save = { "Save", save_document };
    struct button quit = { "Quit", quit_app };

    button_click(&save);   /* prints: [Save] clicked -> saving document... */
    button_click(&quit);   /* prints: [Quit] clicked -> quitting application... */

    printf("\nThe button machinery is identical for both; only the stored\n");
    printf("handler differs. That's a function pointer doing event handling.\n");
    return 0;
}

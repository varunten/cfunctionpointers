/* 03_like_file_ops.c
 *
 * Recognizing the ops-struct / vtable pattern -- the shape of the Linux kernel's
 * struct file_operations. A struct full of function pointers; each "method"
 * takes the object first (self); unsupported slots are NULL and the caller
 * checks before calling.
 *
 * Build: gcc 03_like_file_ops.c -o rfops && ./rfops
 */
#include <stdio.h>
#include <string.h>

struct file;   /* forward */

/* The ops struct (like file_operations). NULL slot = "not supported". */
struct file_ops {
    int (*open) (struct file *self);
    int (*read) (struct file *self, char *buf, int n);
    int (*write)(struct file *self, const char *buf, int n);
    int (*close)(struct file *self);
};

/* The "object": data + a pointer to its ops (the vtable). */
struct file {
    const char           *name;
    const struct file_ops *ops;    /* virtual dispatch through here */
    char  data[64];
    int   len;
};

/* A read-only device: implements open/read/close, NO write. */
static int ro_open (struct file *f){ printf("   opened %s\n", f->name); return 0; }
static int ro_read (struct file *f, char *buf, int n){
    int k = f->len < n ? f->len : n;
    memcpy(buf, f->data, (size_t)k); return k;
}
static int ro_close(struct file *f){ printf("   closed %s\n", f->name); return 0; }

static const struct file_ops readonly_ops = {
    .open = ro_open, .read = ro_read, .write = NULL, .close = ro_close
};

/* The kernel-style caller: dispatch through ops, checking NULL for optionals. */
int file_write(struct file *f, const char *buf, int n) {
    if (!f->ops->write) {                 /* optional method check */
        printf("   write not supported on %s\n", f->name);
        return -1;
    }
    return f->ops->write(f, buf, n);
}

int main(void) {
    struct file dev = { "sensor0", &readonly_ops, "42.5C", 5 };
    dev.len = (int)strlen(dev.data);

    dev.ops->open(&dev);                   /* f->ops->open(f) : virtual dispatch */

    char buf[16] = {0};
    int k = dev.ops->read(&dev, buf, sizeof buf - 1);
    printf("   read %d bytes: \"%s\"\n", k, buf);

    file_write(&dev, "nope", 4);           /* NULL slot -> gracefully unsupported */

    dev.ops->close(&dev);

    printf("\nThis is file_operations in miniature: an ops struct of function\n");
    printf("pointers, self-first methods, NULL = optional. The kernel is full\n");
    printf("of these -- now you can read them.\n");
    return 0;
}

#include "debug_alloc.h"

#define DEBUG_NOW

int main(void) {
    dbg_init();

    int* a = dbg_malloc(sizeof(int), "a");
    int* b = dbg_malloc(sizeof(int), "b");
    int* c = dbg_malloc(sizeof(int), "b");
    int* d = dbg_malloc(sizeof(int), "b");
    a = dbg_realloc(a, sizeof(int) * 100, "a");
    b = dbg_realloc(a, sizeof(int) * 111, "b");
    *a = 33;
    *a = 22;
    *a = 32;
    *a = 10;
    dbg_free(a, "a");
    dbg_free(b, "b");
    dbg_free(c, "c");
    dbg_free(d, "d");
    dbg_info();
    dbg_clean();
    return 0;
}

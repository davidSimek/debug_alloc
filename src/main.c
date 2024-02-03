#include "debug_alloc.h"
#include <stdio.h>

int main(void) {
     
    dbg_init();
    puts("init");

    int* a = dbg_malloc(sizeof(int), "a");
    puts("malloc");

    * a = 33;
    printf("a is %d", *a);

    dbg_free(a, "a");
    puts("free");


    dbg_clean();
    puts("clean");

    return 0;
}

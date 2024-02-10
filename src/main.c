#include "debug_alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Initialize debugging environment
    if (!dbg_init()) {
        printf("Error: Debugging environment initialization failed\n");
        return 1;
    }

    // Allocate memory blocks
    int* int_ptr = (int*)dbg_malloc_msg(sizeof(int), "Integer", "Allocating memory for an integer");
    double* double_ptr = (double*)dbg_malloc_msg(sizeof(double), "Double", "Allocating memory for a double");
    char* string_ptr = (char*)dbg_malloc_msg(strlen("Hello") + 1, "String", "Allocating memory for a string");

    // Check if allocations were successful
    if (int_ptr == NULL || double_ptr == NULL || string_ptr == NULL) {
        printf("Error: Memory allocation failed\n");

        // Clean up and exit
        dbg_clean();
        return 1;
    }

    // Assign values
    *int_ptr = 42;
    *double_ptr = 3.14;
    strcpy(string_ptr, "Hello");

    // Reallocate memory
    string_ptr = (char*)dbg_realloc_msg(string_ptr, strlen("Hello, world!") + 1, "String", "Expanding string buffer to accommodate 'Hello, world!'");

    // Check if reallocation was successful
    if (string_ptr == NULL) {
        printf("Error: Memory reallocation failed\n");

        // Clean up and exit
        dbg_clean();
        return 1;
    }

    // Modify the string
    strcat(string_ptr, ", world!");

    // Free memory
    dbg_free_msg(int_ptr, "Integer", "Freeing memory for the integer");
    dbg_free_msg(double_ptr, "Double", "Freeing memory for the double");
    dbg_free_msg(string_ptr, "String", "Freeing memory for the string");

    // Retrieve and print debugging information
    dbg_info();

    // Clean up
    dbg_clean();

    return 0;
}

#ifndef DEBUG_ALLOC
#define DEBUG_ALLOC

#define DEBUG_NOW

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef DEBUG_NOW

void* dbg_malloc(size_t size, const char* data_name) {
    return malloc(size);
}

void* dbg_calloc(size_t count, size_t size, const char* data_name) {
    return calloc(count, size);
}

void* dbg_realloc(void* pointer, size_t new_size, const char* data_name) {
    return realloc(pointer, new_size);
}

void dbg_free(void* pointer, const char* data_name) {
    free(pointer);
}

#else

#define BASE_ELEMENT_COUNT 4
#define DYN_ARR_INCREMENT

typedef enum {
    MALLOC = 1,
    CALLOC = 2,
    REALLOC = 3,
    FREE = 4
} Operation;

typedef struct {
    void* buffer;
    size_t allocated;
    size_t used;
    int elementSize;
} DynamicArray;

typedef struct {
    const char* name;
    DynamicArray* operations;
} NamedData;


static DynamicArray named_data_arr;
// -------------------------------------
// named_data_arr
//       |             \
// named_data          named_data
// (*name, *operations)  (*name, *operations)
//           |                     \
//          [malloc, free,]       [malloc, realloc, free]

// internal functions

bool create_dynamic_array(DynamicArray* thisArray, size_t elementSize) {
    thisArray->buffer = malloc(elementSize * 4);
    if (thisArray->buffer == NULL)
        return false;


    thisArray->allocated = elementSize * 4;
    thisArray->used = 0;
    thisArray->elementSize = elementSize;
    return true;
}

void destroy_dynamic_array(DynamicArray* thisArray) {
    free(thisArray->buffer);
    thisArray->buffer = 0;
}

bool add_to_dynamic_array(DynamicArray* thisArray, void* value) {
    // is full 
    if (thisArray->allocated == thisArray->used) {

        size_t newSize = thisArray->allocated * 1.5f;
        void* newArray = realloc(thisArray->buffer, newSize); 

        // reallocation failed
        if (newArray == 0) {
            free(thisArray->buffer);
            thisArray->buffer = 0;
            thisArray->allocated = 0;
            thisArray->used = 0;
            thisArray->elementSize = 0;
            return false;
        }

        thisArray->buffer = newArray;
        thisArray->allocated = newSize;
    }

    memcpy(&thisArray->buffer[thisArray->used], value, thisArray->elementSize);
    thisArray->used += thisArray->elementSize;
    return true;
}

void* getFromDynamicArray(DynamicArray* thisArray, size_t index) {
    if (index > (thisArray->used / thisArray->elementSize)) {
        return NULL;
    }
    return &((char*)thisArray->buffer)[index * thisArray->elementSize];
}

void dbg_add_operation(const char* data_name, Operation operation) {

    // try to find existing data name
    // HERE NAMED_DATA_ARRAY IS NOT ACCESSIBLE??????
    for (int i = 0; i < named_data_arr.used; ++i) {
        NamedData* named_data = &((NamedData*)named_data_arr.buffer)[i];
        if (strcmp(named_data->name, data_name) == 0) {
            printf("%s is %s\n", named_data->name, data_name);
            // found
            add_to_dynamic_array(named_data->operations, &operation);
            return;
        }
        printf("%s is not %s\n", named_data->name, data_name);
    }

    NamedData new_named_data;
    // set name
    new_named_data.name = data_name;
    // create operations array
    create_dynamic_array(new_named_data.operations, sizeof(char));
    // add operation to operations array
    add_to_dynamic_array(new_named_data.operations, &operation);

    //assign new_named_data to named_data_array
    add_to_dynamic_array(&named_data_arr, &new_named_data);
}

// interface

bool dbg_init() {
    create_dynamic_array(&named_data_arr, sizeof(NamedData));
    printf("%zu is accessible\n", named_data_arr.used);
}

void dbg_clean() {
    for (int i = 0; i < named_data_arr.used; ++i) {
        destroy_dynamic_array(((NamedData*)named_data_arr.buffer)[i].operations);
    }
    destroy_dynamic_array(&named_data_arr);
}

void* dbg_malloc(size_t size, const char* data_name) {
    dbg_add_operation(data_name, MALLOC);
    return malloc(size);
}

void* dbg_calloc(size_t count, size_t size, const char* data_name) {
    dbg_add_operation(data_name, CALLOC);
    return calloc(count, size);
}

void* dbg_realloc(void* pointer, size_t new_size, const char* data_name) {
    dbg_add_operation(data_name, REALLOC);
    return realloc(pointer, new_size);
}

void dbg_free(void* pointer, const char* data_name) {
    dbg_add_operation(data_name, FREE);
    free(pointer);
}

void debug_print_info() {

}

#endif
#endif

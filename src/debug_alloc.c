#define DEBUG_NOW

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "debug_alloc.h"

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

bool dbg_init() { return true; }

void dbg_clean() {}

void dbg_info() {}

void dbg_info_by_name(char* name) {}

#else
typedef enum {
    MALLOC = 1,
    CALLOC = 2,
    REALLOC = 3,
    FREE = 4
} OperationType;

typedef struct {
    OperationType name;
    size_t allocated;
    const char* message;
} Operation;

typedef struct {
    void* buffer;
    size_t allocated;
    size_t used;
    int elementSize;
} DynamicArray;

typedef struct {
    const char* name;
    DynamicArray operations;
} NamedData;



DynamicArray named_data_arr = {.elementSize = 1};
/*
 named_data_arr
       |             \
 named_data          named_data
 (*name, *operations)  (*name, *operations)
           |                              \
         [{malloc, 10}, {free, 0}]       [{malloc, 33}, {realloc, 222}, {free, 0}]
*/

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

void dbg_add_operation(const char* data_name, OperationType operation_name, size_t size, const char* message) {
    Operation operation = {operation_name, size, message};
    for (int i = 0; i < named_data_arr.used / sizeof(NamedData); ++i) {
        NamedData* named_data = &((NamedData*)named_data_arr.buffer)[i];
        if (strcmp(named_data->name, data_name) == 0) {
            // found
            add_to_dynamic_array(&named_data->operations, &operation);
            return;
        }
    }
    NamedData new_named_data = {0};
    // set name
    
    new_named_data.name = data_name;

    // create operations array
    create_dynamic_array(&new_named_data.operations, sizeof(Operation));

    // add operation to operations array
    add_to_dynamic_array(&new_named_data.operations, &operation);

    //assign new_named_data to named_data_array
    add_to_dynamic_array(&named_data_arr, &new_named_data);
}

// INTERFACE

// runtime

bool dbg_init() {
    return create_dynamic_array(&named_data_arr, sizeof(NamedData));
}

void dbg_clean() {
    for (int i = 0; i < named_data_arr.used / sizeof(DynamicArray); ++i) {
        destroy_dynamic_array(&((NamedData*)named_data_arr.buffer)[i].operations);
    }
    destroy_dynamic_array(&named_data_arr);
}

// allocation utilities with message

void* dbg_malloc_msg(size_t size, const char* data_name, const char* message) {
    dbg_add_operation(data_name, MALLOC, size, message);
    return malloc(size);
}

void* dbg_calloc_msg(size_t count, size_t size, const char* data_name, const char* message) {
    dbg_add_operation(data_name, CALLOC, size * count, message);
    return calloc(count, size);
}

void* dbg_realloc_msg(void* pointer, size_t new_size, const char* data_name, const char* message) {
    dbg_add_operation(data_name, REALLOC, new_size, message);
    return realloc(pointer, new_size);
}

void dbg_free_msg(void* pointer, const char* data_name, const char* message) {
    dbg_add_operation(data_name, FREE, 0, message);
    free(pointer);
    pointer = 0;
}

// allocation utilities with no message

void* dbg_malloc(size_t size, const char* data_name) {
    return dbg_malloc_msg(size, data_name, NULL);
}

void* dbg_calloc(size_t count, size_t size, const char* data_name) {
    return dbg_calloc_msg(count, size, data_name, NULL);
}

void* dbg_realloc(void* pointer, size_t new_size, const char* data_name) {
    return dbg_realloc_msg(pointer, new_size, data_name, NULL);
}

void dbg_free(void* pointer, const char* data_name) {
    dbg_free_msg(pointer, data_name, NULL);
}

// logging

void dbg_info() {
    printf("____________________________\n");
    printf("|                           |\n");
    printf("|         DEBUG LOG         |\n");
    for (int i = 0; i < named_data_arr.used / sizeof(NamedData); ++i) {
        printf("|___________________________|\n");
        printf("|                           |\n");
        NamedData currentData = ((NamedData*)named_data_arr.buffer)[i];

        size_t last_index = currentData.operations.used / sizeof(Operation);
        Operation last_operation = ((Operation*)currentData.operations.buffer)[last_index - 1];

        if (last_operation.name == FREE)
            printf("| %s (\033[0;32mfreed\033[0m)\n", currentData.name);
        else
            printf("| %s (\033[1;31mnot freed\033[0m)\n", currentData.name);

        for (int j = 0; j < currentData.operations.used / sizeof(Operation); ++j) {
            Operation operation = ((Operation*)currentData.operations.buffer)[j];
            if (operation.name == MALLOC) {
                printf("| \t%d: MALLOC(%zuB)\t", j, operation.allocated);
                if (operation.message == NULL)
                    printf("\n");
                else
                    printf("[%s]\n", operation.message);
            } else if (operation.name == REALLOC) {
                printf("| \t%d: REALLOC(%zuB)\t", j, operation.allocated);
                if (operation.message == NULL)
                    printf("\n");
                else
                    printf("[%s]\n", operation.message);
            } else if (operation.name == CALLOC) {
                printf("| \t%d: CALLOC(%zuB)\t", j, operation.allocated);
                if (operation.message == NULL)
                    printf("\n");
                else
                    printf("[%s]\n", operation.message);
            } else if (operation.name == FREE) {
                printf("| \t%d: FREE\t", j);
                if (operation.message == NULL)
                    printf("\n");
                else
                    printf("[%s]\n", operation.message);
            }
        }
    }
    printf("|___________________________\n");
}

void dbg_info_by_name(char* name) {
    printf("____________________________\n");
    printf("|                           |\n");
    printf("|         DEBUG LOG         |\n");
    for (int i = 0; i < named_data_arr.used / sizeof(DynamicArray); ++i) {
        printf("|___________________________|\n");
        NamedData currentData = ((NamedData*)named_data_arr.buffer)[i];

        if (strcmp(currentData.name, name) != 0)
            continue;

        size_t last_index = currentData.operations.used / sizeof(Operation);
        Operation last_operation = ((Operation*)currentData.operations.buffer)[last_index - 1];

        if (last_operation.name == FREE)
            printf("| %s (\033[0;32mfreed\033[0m)\n", currentData.name);
        else
            printf("| %s (\033[1;31mnot freed\033[0m)\n", currentData.name);

        printf("|___________________________\n");
        printf("|                           |\n");
        for (int j = 0; j < currentData.operations.used / sizeof(Operation); ++j) {
            Operation operation = ((Operation*)currentData.operations.buffer)[j];
            if (operation.name == MALLOC) {
                printf("| \t%d: MALLOC(%zuB)\t", j, operation.allocated);
                if (operation.message == NULL)
                    printf("\n");
                else
                    printf("[%s]\n", operation.message);
            } else if (operation.name == REALLOC) {
                printf("| \t%d: REALLOC(%zuB)\t", j, operation.allocated);
                if (operation.message == NULL)
                    printf("\n");
                else
                    printf("[%s]\n", operation.message);
            } else if (operation.name == CALLOC) {
                printf("| \t%d: CALLOC(%zuB)\t", j, operation.allocated);
                if (operation.message == NULL)
                    printf("\n");
                else
                    printf("[%s]\n", operation.message);
            } else if (operation.name == FREE) {
                printf("| \t%d: FREE\t", j);
                if (operation.message == NULL)
                    printf("\n");
                else
                    printf("[%s]\n", operation.message);
            }
        }
    }
    printf("|___________________________\n");
}

#endif

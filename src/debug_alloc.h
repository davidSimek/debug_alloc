#ifndef DEBUG_ALLOC
#define DEBUG_ALLOC

#define DEBUG_NOW

#include <stdlib.h>
#include <stdbool.h>

bool dbg_init();
void dbg_clean();

void* dbg_malloc(size_t size, const char* data_name);
void* dbg_calloc(size_t count, size_t size, const char* data_name);
void* dbg_realloc(void* pointer, size_t new_size, const char* data_name);
void  dbg_free(void* pointer, const char* data_name);

void* dbg_malloc_msg(size_t size, const char* data_name, const char* message);
void* dbg_calloc_msg(size_t count, size_t size, const char* data_name, const char* message);
void* dbg_realloc_msg(void* pointer, size_t new_size, const char* data_name, const char* message);
void  dbg_free_msg(void* pointer, const char* data_name, const char* message);


void dbg_info();
void dbg_info_by_name(char* name);

#endif

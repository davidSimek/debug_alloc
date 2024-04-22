# C memory tracking library

The Debug Allocator Library (`debug_alloc`) is a lightweight C library for debugging memory allocation in C programs. It provides functions for tracking memory allocation and deallocation, as well as debugging information for easier identification of memory-related issues.

## Features

- Initialization and cleanup functions for setting up and cleaning up the debug allocator.
- Memory allocation functions (`malloc`, `calloc`, `realloc`, `free`) with debugging support.
- Optional message parameter for providing additional context during memory allocation.
- Information retrieval functions for debugging purposes.

## Usage

To use the Debug Allocator Library in your project, include the `debug_alloc.h` header file in your source files and link against the library during compilation. Here's an example of how to use the library:

```c
#include "debug_alloc.h"

int main() {
    // Initialize the debug allocator
    dbg_init();

    // Allocate memory
    int* ptr = dbg_malloc(sizeof(int), "integer");

    // Free memory
    dbg_free(ptr, "integer");

    // Print out whole allocation history
    dbg_info();

    // Clean up the debug allocator
    dbg_clean();

    return 0;
}
```

## Example
```c
#include "debug_alloc.h"
#include <stdio.h>

typedef struct {
    void (*make_sound)(void*);
} Animal;

void bark(void* animal) {
    printf("BARK!!!\n");
}
void meow(void* animal) {
    printf("MEOW\n");
}

void animal_init(Animal* animal, void(*sound)(void*)) {
    animal->make_sound = sound;
}

int main(void) {
    
    dbg_init();

    Animal* dog = dbg_malloc(sizeof(Animal), "dog");
    Animal* cat = dbg_malloc(sizeof(Animal), "cat");

    animal_init(dog, bark);
    animal_init(cat, meow);

    dog->make_sound(dog);
    cat->make_sound(cat);

    dbg_free(cat, "cat");
    dbg_free(dog, "dog");

    dbg_info();

    dbg_clean();

    return 0;
}

```
output:
```
BARK!!!
MEOW
____________________________
|                           |
|         DEBUG LOG         |
|___________________________|
|                           |
| dog (freed)
| 	0: MALLOC(8B)	
| 	1: FREE	
|___________________________|
|                           |
| cat (freed)
| 	0: MALLOC(8B)	
| 	1: FREE	
|___________________________
```

## Compilation

To compile your project with the Debug Allocator Library, ensure that the `debug_alloc.c` source file is included in your compilation command:

```bash
gcc -o my_program my_program.c debug_alloc.c
```

## Configuration

The Debug Allocator Library can be configured by defining the `DEBUG_ALLOC` macro. When this macro is defined, library will work the way you expect it. If it is not defined, library acts as wrapper around `malloc`, `calloc`, `realloc` and `free`, that does nothing special, but calling coresponding fucntion. You can define this macro globally in your project or using compiler flags:

```bash
gcc -DDEBUG_ALLOC -o my_program my_program.c debug_alloc.c
```

## Documentation

For detailed documentation on the usage and API of the Debug Allocator Library, please refer to the comments in the `debug_alloc.h` header file.

## License
This project uses Apache License 2.0

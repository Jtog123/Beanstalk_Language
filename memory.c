#include <stdlib.h>

#include "memory.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if(newSize == 0) {
        free(pointer);
        return NULL;
    }

    //changes the size of a previosuly allocated memory block
    //realloc, because i need the old contents copied, tries to resize memory in place
    // if it cant reates new block of given size, and copies contents over
    void* result = realloc(pointer, newSize);
    if(result == NULL) exit(1);
    return result;
}
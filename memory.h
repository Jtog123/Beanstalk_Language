#ifndef BEANSTALK_MEMORY_H
#define BEANSTALK_MEMORY_H

#include "commonlibs.h"



/* -------------------------- MACROS -------------------------- */

//Should i move these into the Chunk class??????

/*
Macros are handled by preprocessor before compilation
Macros ONLY substitute text, the preprocessor does NOT know about c++ syntax, its just pastes text
The book uses Macros like this:

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

Macros wil be used as I need to reuse them to dynamically create different types like Ints, Values, structs, etc
*/


/* This Macro calculates a new capacity based on the current capacity. It doubles the capacity of the chunk when capacity is hit */
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)


/*pass a type make it a pointer, pass the code to pointer, pass the old capacity of elements, pass the new capacity */
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

/* frees memory */
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);


#endif
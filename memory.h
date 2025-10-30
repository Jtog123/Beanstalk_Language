#ifndef BEANSTALK_MEMORY_H
#define BEANSTALK_MEMORY_H

#include "commonlibs.h"
//#include "object.h"



/* -------------------------- MACROS -------------------------- */

//Should i move these into the Chunk class??????

/*
Macros are handled by preprocessor before compilation
Macros ONLY substitute text, the preprocessor does NOT know about C syntax, its just pastes text
The book uses Macros like this:

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

Macros wil be used as I need to reuse them to dynamically create different types like Ints, Values, structs, etc
*/
#define ALLOCATE(type, count) \
    ((type*)reallocate(NULL, 0, sizeof(type) * (count) ))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

/* This Macro calculates a new capacity based on the current capacity. It doubles the capacity of the chunk when capacity is hit */
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)





/*
Call the function reallocate pass the Chunks code, its old capacity, and its new capacity 
Reallocate returns a void pointer upon returning from the call to reallocate, GROW_ARRAY casts the pointer to the given type
for ex. unit8_t*

uint8_t sizeof(type) returns 1 byte 
*/
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

/* frees memory */
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);
void freeObjects();


#endif
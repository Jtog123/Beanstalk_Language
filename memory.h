#ifndef BEANSTALK_MEMORY_H
#define BEANSTALK_MEMORY_H

#include "commonlibs.h"



/* -------------------------- MEMORY FUNCTIONS -------------------------- */

//Should i move these into the Chunk class??????

/*
Macros are handled by preprocessor before compilation
Macros ONLY substitute text, the preprocessor does NOT know about c++ syntax, its just pastes text
The book uses Macros like this:

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

I could've just copied the Macros but from my research it's generally a bad practice as they are almost impossible to debug. I also wanted to CPPify this code
I needed a way to to access just type information, in CPP templates seem to be the best way to gather dynamic type information.
*/

/* calculates a new capacity based on the current capacity. It doubles the capacity of the chunk when capacity is hit */
int GROW_CAPACITY(int capacity) {
    return capacity < 8 ? 8 : (capacity) * 2;
}

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

/* Dynamically grows our Chunk/Array*/
template <typename T>
T GROW_ARRAY(T, uint8_t* pointer, int oldCount, int newCount) {
    (T*)reallocate(pointer, sizeof(T) * (oldCount), sizeof(T) * (newCount) );
}

template<typename T>
void* FREE_ARRAY(T, uint8_t* pointer, int oldCount) {
    reallocate(pointer, sizeof(T) * (oldCount), 0);
}


/*
/* This Macro calculates a new capacity based on the current capacity. It doubles the capacity of the chunk when capacity is hit 
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

/*pass a type make it a pointer, pass the code to pointer, pass the old capacity of elements, pass the new capacity 
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

/*frees memory
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)
*/

//void* reallocate(void* pointer, size_t oldSize, size_t newSize);




#endif
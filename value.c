#include <stdio.h>

#include "memory.h"
#include "value.h"

void initValueArray(ValueArray* array) {
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void writeValueArray(ValueArray* array, Value value) {
    // if we are at capacity
    if(array->capacity < array->count + 1) {
        int oldCapacty = array->capacity;
        //get the new cpacity
        array->capacity = GROW_CAPACITY(oldCapacty);
        //Grow the array
        array->values = GROW_ARRAY(Value, array->values, oldCapacty, array->capacity);

    }
    // append the value
    array->values[array->count] = value;

    //incerment the number of items in the array
    array->count++;
}

void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value) {
    printf("%g", value);
}
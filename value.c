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
    switch(value.type) {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true": "false");
            break;
        case VAL_NIL:
            printf("nil"); break;
        case VAL_NUMBER:
            printf("%g", AS_NUMBER(value));
            break;

    }
}

bool valuesEqual(Value a, Value b) {
    if(a.type != b.type) {
        return false;
    }
    //we have the same type
    switch(a.type) {
        case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL: return true;
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        default: return false;
    }
}
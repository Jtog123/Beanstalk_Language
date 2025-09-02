#ifndef BEANSTOCK_VALUE_H
#define BEANSTOCK_VALUE_H

#include "commonlibs.h"



typedef double Value;

typedef struct {
    int count;
    int capacity;
    Value* values;
} ValueArray;

//Functions
void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);


void printValue(Value value);

#endif
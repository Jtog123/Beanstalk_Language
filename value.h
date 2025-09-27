#ifndef BEANSTOCK_VALUE_H
#define BEANSTOCK_VALUE_H

#include "commonlibs.h"

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,

} ValueType;

//tagged union, type:value
typedef struct {
    ValueType type;
    union 
    {
        bool boolean;
        double number;
    } as;  
} Value;

//Macros type validation
#define IS_BOOL(value)      ((value).type == VAL_BOOL)
#define IS_NIL(value)       ((value).type == VAL_NIL)
#define IS_NUMBER(value)    ((value).type == VAL_NUMBER)

// Macros extract C Values from Value Structs
// Value Struct --> C value
#define AS_BOOL(value)      ((value).as.boolean)
#define AS_NUMBER(value)    ((value).as.number)

// Macros Create Value structs. 
// C value --> Value Struct
#define BOOL_VAL(value)     ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL             ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value)   ((Value){VAL_NUMBER, {.number = value}})

typedef struct {
    int count;
    int capacity;
    Value* values;
} ValueArray;

bool valuesEqual(Value a, Value b);

//Functions
void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);


void printValue(Value value);

#endif
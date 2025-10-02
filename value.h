#ifndef BEANSTOCK_VALUE_H
#define BEANSTOCK_VALUE_H

#include "commonlibs.h"

//declars a struct named Obj, then gives an alias Obj
// thats why written twice
typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ
} ValueType;

//tagged union, type:value
typedef struct {
    ValueType type;
    union 
    {
        bool boolean;
        double number;
        Obj* obj;
    } as;  
} Value;

//Macros type validation
#define IS_BOOL(value)      ((value).type == VAL_BOOL)
#define IS_NIL(value)       ((value).type == VAL_NIL)
#define IS_NUMBER(value)    ((value).type == VAL_NUMBER)
#define IS_OBJ(value)       ((value).type == VAL_OBJ)

// Macros extract C Values from Value Structs
// Value Struct --> C value
#define AS_OBJ(value)       ((value).as.obj)
#define AS_BOOL(value)      ((value).as.boolean)
#define AS_NUMBER(value)    ((value).as.number)

// Macros Create Value structs. 
// C value --> Value Struct
#define BOOL_VAL(value)     ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL             ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value)   ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)      ((Value){VAL_OBJ, {.obj = (Obj*)object}})

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
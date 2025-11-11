#ifndef BEANSTALK_OBJECT_H
#define BEANSTALK_OBJECT_H

#include "commonlibs.h"
#include "value.h"
#include "chunk.h"


//grab type tag from the object
#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_FUNCTION(value)  isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value)    isObjType(value, OBJ_NATIVE)
#define IS_STRING(value)    isObjType(value, OBJ_STRING)

#define AS_FUNCTION(value)  ((ObjFunction*)AS_OBJ(value))
//Take Value struct, first returns an ObjString*, second returns char array itself
#define AS_STRING(value)    ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)   (((ObjString*)AS_OBJ(value))->chars)
#define AS_NATIVE(value) \
    (((ObjNative*)AS_OBJ(value))->function)



typedef enum {
    OBJ_FUNCTION,
    OBJ_NATIVE,
    OBJ_STRING,
} ObjType;

struct Obj {
    ObjType type;
    struct Obj* next;
};

typedef struct {
    Obj obj;
    int arity;
    Chunk chunk;
    ObjString* name;    
} ObjFunction;

typedef Value (*NativeFn)(int argCount, Value* args);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

struct ObjString {
    Obj obj; // ObjString is an Obj, so needs state all Objs share
    int length;
    char* chars;
    uint32_t hash;
};

ObjFunction* newFunction();
ObjNative* newNative(NativeFn fnuction);

ObjString* takeString(char* chars, int length);

ObjString* copyString(const char* chars, int length);
void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
    //do we have an object and is that object the type we passed in?
    return IS_OBJ(value) && AS_OBJ(value)->type == OBJ_STRING;
}

#endif
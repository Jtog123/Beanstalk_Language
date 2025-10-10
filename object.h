#ifndef BEANSTALK_OBJECT_H
#define BEANSTALK_OBJECT_H

#include "commonlibs.h"
#include "value.h"


//grab type tag from the object
#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_STRING(value)    isObjType(value, OBJ_STRING)

//Take Value struct, first returns an ObjString*, second returns char array itself
#define AS_STRING(value)    ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)   (((ObjString*)AS_OBJ(value))->chars)



typedef enum {
    OBJ_STRING,
} ObjType;

struct Obj {
    ObjType type;
    struct Obj* next;
};

struct ObjString {
    Obj obj; // ObjString is an Obj, so needs state all Objs share
    int length;
    char* chars;
    uint32_t hash;
};

ObjString* takeString(char* chars, int length);

ObjString* copyString(const char* chars, int length);
void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
    //do we have an object and is that object the type we passed in?
    return IS_OBJ(value) && AS_OBJ(value)->type == OBJ_STRING;
}

#endif
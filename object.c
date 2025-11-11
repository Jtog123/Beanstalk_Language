#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "table.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0 , size);
    object->type = type;

    object->next = vm.objects;
    vm.objects = object; //insert at head

    return object;
}

//Set function up in blank state, get filled later
ObjFunction* newFunction() {
    ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    initChunk(&function->chunk);
    return function;
}

ObjNative* newNative(NativeFn function) {
    ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->function = function;
    return native;
}


static ObjString* allocateString(char* chars, int length, uint32_t hash) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING); // allocate an object of type string, also implicitly casting Obj* to ObjString*
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    tableSet(&vm.strings, string, NIL_VAL);
    return string;
}

//FNV-1a hash function
static uint32_t hashString(const char* key, int length) {
    uint32_t hash = 2166136261u;
    for(int i = 0 ; i < length; i++) {
        hash ^= (uint8_t)key[i]; // XOR
        hash *= 16777619;
    }
    return hash;
}


ObjString* takeString(char* chars, int length) {
    uint32_t hash = hashString(chars,length);
    ObjString* interned = tableFindString(&vm.strings, chars,length,hash);
    if(interned != NULL) {
        FREE_ARRAY(chars, chars, length + 1);
        return interned;
    }
    return allocateString(chars,length, hash);
}

//Allocate an array on heap, just big enough for strings chars
//copy the contents over into newly allocated array
//tack on null terminator
ObjString* copyString(const char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    ObjString* interned = tableFindString(&vm.strings, chars, length,hash);
    if(interned != NULL) return interned;
    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length); // copy (dest, src, amount)
    heapChars[length] = '\0';
    return allocateString(heapChars, length, hash);
}

static void printFunction(ObjFunction* function) {
    if(function->name == NULL) {
        printf("<script>");
        return;
    }
    printf("<fn %s>", function->name->chars);
}

void printObject(Value value) {
    switch(OBJ_TYPE(value)) {
        case OBJ_FUNCTION:
            printFunction(AS_FUNCTION(value));
            break;
        case OBJ_NATIVE:
            printf("<native fn>");
            break;
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}
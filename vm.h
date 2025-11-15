#ifndef BEANSTALK_VM_H
#define BEANSTALK_VM_H

#include "chunk.h"
#include "value.h"
#include "table.h"
#include "object.h"


#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)
//#define STACK_MAX 256

typedef struct {
    ObjClosure* closure;
    uint8_t* ip;
    Value* slots;

} CallFrame;

typedef struct {
    //Chunk* chunk;
    //uint8_t* ip;

    //treat like a stack
    CallFrame frames[FRAMES_MAX];
    //number of ongoing function calls
    int frameCount;

    Value stack[STACK_MAX]; // pointer to top of stack, points where next item will be pushed
    Value* stackTop;
    Table globals;
    Table strings;
    ObjUpvalue* openUpvalues;
    size_t bytesAllocated;
    size_t nextGC;
    Obj* objects;
    int yellowCount;
    int yellowCapacity;
    Obj** yellowStack;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);

//InterpretResult interpret(Chunk* chunk);

void push(Value value);
Value pop();



#endif
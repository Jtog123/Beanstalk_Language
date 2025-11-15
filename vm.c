
#include <string.h>
#include <time.h>
#include "commonlibs.h"
#include "vm.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "compiler.h"
#include <stdio.h>
#include <stdarg.h>



//Global variable, when implementing vm functions prevents us from having to pass a pointer to a VM in every function
VM vm;

static Value clockNative(int argCount, Value* args) {
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

static void resetStack() {
    //set the pointer to the top of the stack, to the first elemnt pointed at on the stack
    vm.stackTop = vm.stack;
    vm.frameCount = 0;
    vm.openUpvalues = NULL;
}

//passing variadic args, meaning any arbitrary N number of args are allwoed
static void runtimeError(const char* format , ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    for (int i = vm.frameCount - 1; i >= 0; i--) {
        CallFrame* frame = &vm.frames[i];
        ObjFunction* function = frame->closure->function;
        size_t instruction = frame->ip - function->chunk.code - 1;
        fprintf(stderr, "[line %d] in ", 
            function->chunk.lines[instruction]);
        if (function->name == NULL) {
            fprintf(stderr, "script\n");
        } else {
            fprintf(stderr, "%s()\n", function->name->chars);
        }
    }
    resetStack();
    

}

static void defineNative(const char* name, NativeFn function) {
    push(OBJ_VAL(copyString(name, (int)strlen(name))));
    push(OBJ_VAL(newNative(function)));
    tableSet(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
    pop();
    pop();
}

void initVM() {
    resetStack();
    vm.objects = NULL;
    vm.bytesAllocated = 0;
    vm.nextGC = 1024 * 1024;

    vm.yellowCount = 0;
    vm.yellowCapacity = 0;
    vm.yellowStack = NULL;

    initTable(&vm.globals);
    initTable(&vm.strings);

    defineNative("clock", clockNative);
    
}

void freeVM() {
    freeTable(&vm.globals);
    freeTable(&vm.strings);
    freeObjects();
}

void push(Value value) {
    //should add check here if we are at capcity, then grow dynamically

    //push to the top
    *vm.stackTop = value;
    //increment the pointer
    vm.stackTop++;
}


//after we pop values we end up just overwriting them when we push again as we dont need them.

Value pop() {
    //move the pointer back
    vm.stackTop--;
    //return the top
    return *vm.stackTop;
}

//distanec arg = how far down from the top of the stack to go, zero top, one is one slot down, etc
static Value peek(int distance) {
    return vm.stackTop[-1 - distance];
}

static bool call(ObjClosure* closure, int argCount) {

    if(argCount != closure->function->arity) {
        runtimeError("Expected %d args but got %d.", closure->function->arity, argCount);
        return false;
    }

    if(vm.frameCount == FRAMES_MAX) {
        runtimeError("Stack overflow");
        return false;
    }

    CallFrame* frame = &vm.frames[vm.frameCount++];
    frame->closure = closure;
    frame->ip = closure->function->chunk.code;
    //frame->function = function;
    //frame->ip = function->chunk.code;
    //assign realtive slots
    frame->slots = vm.stackTop - argCount - 1;
    return true;
}

static bool callValue(Value callee, int argCount) {
    if(IS_OBJ(callee)) {
        switch(OBJ_TYPE(callee)) {
            case OBJ_CLOSURE:
                return call(AS_CLOSURE(callee), argCount);
            case OBJ_NATIVE : {
                NativeFn native = AS_NATIVE(callee);
                Value result = native(argCount, vm.stackTop - argCount);
                vm.stackTop -= argCount + 1;
                push(result);
                return true;
            }
            default:
                break;
        }
    }
    runtimeError("Can only call functions and classes.");
    return false;
}
static ObjUpvalue* captureUpvalue(Value* local) {
    ObjUpvalue* prevUpvalue = NULL;
    ObjUpvalue* upvalue = vm.openUpvalues;
    while (upvalue != NULL && upvalue->location > local) {
        prevUpvalue = upvalue;
        upvalue = upvalue->next;
    }

    if (upvalue != NULL && upvalue->location == local) {
        return upvalue;
    }
    ObjUpvalue* createdUpvalue = newUpvalue(local);

    createdUpvalue->next = upvalue;

    if (prevUpvalue == NULL) {
        vm.openUpvalues = createdUpvalue;
    } else {
        prevUpvalue->next = createdUpvalue;
    }
    
    return createdUpvalue;
}

static void closeUpvalues(Value* last) {
    while (vm.openUpvalues != NULL && vm.openUpvalues->location >= last) {
        ObjUpvalue* upvalue = vm.openUpvalues;
        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        vm.openUpvalues = upvalue->next;
    }
}

// nil and flase are falsey, all other value behave like true
// if its nil its falsey
//if its the value is a bool AND that boolean value is False
// return true its a falsey value
static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
    ObjString* b = AS_STRING(peek(0));
    ObjString* a = AS_STRING(peek(1));

    int length = a->length + b->length; // calc new length
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);; //dest,src,length -> copy string a
    memcpy(chars + a->length, b->chars, b->length); // copy string b into chars
    chars[length] = '\0';

    ObjString* result = takeString(chars, length);
    pop();
    pop();
    push(OBJ_VAL(result));
}



//static somewhat modulates the code, run can only be called within vm.c
static InterpretResult run() {
    CallFrame* frame = &vm.frames[vm.frameCount - 1];

////////////////////////////////// MACROS /////////////////////////////////////////////////////

// get first byte pointed at by instruction pointer. then incremenet the pointer
#define READ_BYTE() (*frame->ip++)

//READ_BYTE returns the next byte which is an index in the CODE array. This index allows us to index into VALUUES array and retieve the constant
#define READ_CONSTANT() (frame->closure->function->chunk.constants.values[READ_BYTE()])

//pulls 2 bytes from the chunk and builds a 16bit unsigned int
#define READ_SHORT() \
    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))

//reads 1 byte operand, treats as index into constant table (hashmap), retusns strnig at that index.
#define READ_STRING() AS_STRING(READ_CONSTANT())

//Boilerplate all binary operations follow this format.
// make sure the top two items on vm's stack are numbers
#define BINARY_OP(valueType, op) \
    do {\
        if(!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers"); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        double b = AS_NUMBER(pop()); \
        double a = AS_NUMBER(pop()); \
        push(valueType(a op b)); \
    } while(false)

//////////////////////////////////////////////////////////////////////////////////////////////

    //each time through the loop we read and process 1 bytecode instruciton
    for(;;) {

    //print each instruction right before execution, so we can see, uses a little pointer arithmetic
    #ifdef DEBUG_TRACE_EXECUTION
            disassembleInstruction(&frame->closure->function->chunk, (int)(frame->ip - frame->closure->function->chunk.code));

            //a look into the VM's stack
            printf("          ");
            for(Value* slot = vm.stack; slot < vm.stackTop; slot++) { // slot++ moves pointer by 8 bytes
                printf("[ ");
                printValue(*slot);
                printf(" ]");
            }
            printf("\n");
    #endif


        uint8_t instruction;
        switch(instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                //printValue(constant);
                //printf("\n");
                break;
            }
            case OP_NIL:
                push(NIL_VAL);
                break;
            case OP_TRUE:
                push(BOOL_VAL(true));
                break;
            case OP_FALSE:
                push(BOOL_VAL(false));
                break;
            case OP_POP:
                pop();
                break;
            case OP_GET_GLOBAL: {
                //var bacon = 5;
                //get a string like 'bacon' use as a lookup into hashmap
                //if we can find the key the variable has not been defined, and we throw an error
                // otherwise we succesfully look up the value and push it onto the stack. (the 5) 
                ObjString* name = READ_STRING();
                Value value;
                if(!tableGet(&vm.globals, name, &value)) {
                    runtimeError("Undefined variable '%s'.", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
                //value gets written in tableGet()
                push(value);
                break;
            }
            case OP_DEFINE_GLOBAL: {
                //var bacon = 5, "bacon" store in chunks values array, we grab it
                ObjString* name = READ_STRING();

                // pass in the table, key, value at top of the stack.
                //take value at top of the stack stoer it in hashtable with 'name' as key
                tableSet(&vm.globals, name, peek(0)); 
                
                pop();
                break;
            }
            case OP_GET_LOCAL: {
                //get the index of the value from bytecode
                uint8_t slot = READ_BYTE();
                //given that index, push that value to the stacks slot
                push(frame->slots[slot]);
                break;
            }
            case OP_SET_LOCAL: {
                uint8_t slot = READ_BYTE();
                frame->slots[slot] = peek(0);
                break;
            }
            case OP_SET_GLOBAL: {
                ObjString* name = READ_STRING();
                if(tableSet(&vm.globals, name, peek(0))) {
                    tableDelete(&vm.globals, name);
                    runtimeError("undefined variable '%s'.", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_GET_UPVALUE: {
                uint8_t slot = READ_BYTE();
                push(*frame->closure->upvalues[slot]->location);
                break;
            }
            case OP_SET_UPVALUE: {
                uint8_t slot = READ_BYTE();
                *frame->closure->upvalues[slot]->location = peek(0);
                break;
            }
            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();

                push(BOOL_VAL(valuesEqual(a, b)));
                break;
            }
            case OP_GREATER: {
                BINARY_OP(BOOL_VAL, >);
                break;
            }
            case OP_LESS: {
                BINARY_OP(BOOL_VAL, <);
                break;
            }
            case OP_ADD: {
                if(IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                    // if top two objects on vm stack are strings concat them
                    concatenate();
                }
                else if(IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                    double b = AS_NUMBER(pop());
                    double a = AS_NUMBER(pop());
                    push(NUMBER_VAL(a + b));
                } else {
                    runtimeError("OPerands must be two numbers or two strings");
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SUBTRACT: {
                BINARY_OP(NUMBER_VAL, -);
                break;
            }
            case OP_MULTIPLY: {
                BINARY_OP(NUMBER_VAL, *);
                break;
            }
            case OP_DIVIDE: {
                BINARY_OP(NUMBER_VAL, /);
                break;
            }
            case OP_NOT: {
                push(BOOL_VAL(isFalsey(pop())));
                break;
            }
            case OP_NEGATE: {
                //check if top of stack is number
                if(!IS_NUMBER(peek(0))) {
                    runtimeError("Operand must be a number");
                    return INTERPRET_RUNTIME_ERROR;
                }
                // get the constant from the stack, and negate it
                //Value constant = -AS_NUMBER(pop());
                //push back to the stack
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            }
            case OP_PRINT: {
                printValue(pop());
                printf("\n");
                break;
            }
            case OP_JUMP: {
                uint16_t offset = READ_SHORT();
                frame->ip += offset;
                break;
            }
            case OP_JUMP_IF_FALSE: {
                uint16_t offset = READ_SHORT();
                //if the value at the top of the stackis falsey,
                //skip a specified number of instructions
                //otherwise continue as normal
                if(isFalsey(peek(0))) {
                    frame->ip += offset;
                }
                break;
            }
            case OP_LOOP: {
                uint16_t offset = READ_SHORT();
                frame->ip -= offset;
                break;
            }
            case OP_CALL: {
                int argCount = READ_BYTE();
                if(!callValue(peek(argCount), argCount)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                frame = &vm.frames[vm.frameCount - 1];
                break;
            }
            case OP_CLOSURE: {
                ObjFunction* function = AS_FUNCTION(READ_CONSTANT());
                ObjClosure* closure = newClosure(function);
                push(OBJ_VAL(closure));
                for (int i = 0; i < closure->upvalueCount; i++) {
                    uint8_t isLocal = READ_BYTE();
                    uint8_t index = READ_BYTE();
                    if (isLocal) {
                        closure->upvalues[i] = captureUpvalue(frame->slots + index);
                    } else {
                        closure->upvalues[i] = frame->closure->upvalues[index];
                    }
                }
                break;
            }
            case OP_CLOSE_UPVALUE:
                closeUpvalues(vm.stackTop - 1);
                pop();
                break;
            case OP_RETURN: {
                Value result = pop();
                closeUpvalues(frame->slots); // right spot?
                vm.frameCount--;
                if(vm.frameCount == 0) {
                    pop();
                    return INTERPRET_OK;
                }
                
                vm.stackTop = frame->slots;
                push(result);
                frame = &vm.frames[vm.frameCount - 1];
                break;
            //    printValue(pop());
            //    printf("\n");
                
            }

            //code
        }
    }

    //undefine the macros after were done with it
    #undef READ_CONSTANT
    #undef READ_STRING
    #undef READ_BYTE
    #undef READ_SHORT
    #undef BINARY_OP


}



InterpretResult interpret(const char* source) {
    ObjFunction* function = compile(source);
    if(function == NULL) return INTERPRET_COMPILE_ERROR;

    push(OBJ_VAL(function));
    ObjClosure* closure = newClosure(function);
    pop();
    push(OBJ_VAL(closure));
    call(closure, 0);
    //call(function, 0); // sets up first frame for top level code
    //CallFrame* frame = &vm.frames[vm.frameCount++];
    //frame->function = function;
    //frame->ip = function->chunk.code;
    //frame->slots = vm.stack;

    //Chunk chunk;
    //initChunk(&chunk);

    /*
    if(!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    //let the vm runs this chunk of instructions
    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;
    */

    //InterpretResult result = run();

    //freeChunk(&chunk);
    return run();


}





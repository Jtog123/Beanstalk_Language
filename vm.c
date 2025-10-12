#include "commonlibs.h"
#include "vm.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "compiler.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

//Global variable, when implementing vm functions prevents us from having to pass a pointer to a VM in every function
VM vm;

static void resetStack() {
    //set the pointer to the top of the stack, to the first elemnt pointed at on the stack
    vm.stackTop = vm.stack;
}

//passing variadic args, meaning any arbitrary N number of args are allwoed
static void runtimeError(const char* format , ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
    

}

void initVM() {
    resetStack();
    vm.objects = NULL;
    initTable(&vm.globals);
    initTable(&vm.strings);
    
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

// nil and flase are falsey, all other value behave like true
// if its nil its falsey
//if its the value is a bool AND that boolean value is False
// return true its a falsey value
static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
    ObjString* b = AS_STRING(pop());
    ObjString* a = AS_STRING(pop());

    int length = a->length + b->length; // calc new length
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);; //dest,src,length -> copy string a
    memcpy(chars + a->length, b->chars, b->length); // copy string b into chars
    chars[length] = '\0';

    ObjString* result = takeString(chars, length);
    push(OBJ_VAL(result));
}



//static somewhat modulates the code, run can only be called within vm.c
static InterpretResult run() {

////////////////////////////////// MACROS /////////////////////////////////////////////////////

// get first byte pointed at by instruction pointer. then incremenet the pointer
#define READ_BYTE() (*vm.ip++)

//READ_BYTE returns the next byte which is an index in the CODE array. This index allows us to index into VALUUES array and retieve the constant
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

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
            disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));

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
                printValue(constant);
                printf("\n");
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
            case OP_SET_GLOBAL: {
                ObjString* name = READ_STRING();
                if(tableSet(&vm.globals, name, peek(0))) {
                    tableDelete(&vm.globals, name);
                    runtimeError("undefined variable '%s'.", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
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
            case OP_RETURN: {
                return INTERPRET_OK;
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
    #undef BINARY_OP


}



InterpretResult interpret(const char* source) {

    Chunk chunk;
    initChunk(&chunk);

    if(!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    //let the vm runs this chunk of instructions
    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;


}






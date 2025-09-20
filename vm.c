#include "commonlibs.h"
#include "vm.h"
#include "debug.h"
#include "compiler.h"
#include <stdio.h>

//Global variable, when implementing vm functions prevents us from having to pass a pointer to a VM in every function
VM vm;

static void resetStack() {
    //set the pointer to the top of the stack, to the first elemnt pointed at on the stack
    vm.stackTop = vm.stack;
}

void initVM() {
    resetStack();
}

void freeVM() {

}

//static somewhat modulates the code, run can only be called within vm.c
static InterpretResult run() {

////////////////////////////////// MACROS /////////////////////////////////////////////////////

// get first byte pointed at by instruction pointer. then incremenet the pointer
#define READ_BYTE() (*vm.ip++)

//READ_BYTE returns the next byte which is an index in the CODE array. This index allows us to index into VALUUES array and retieve the constant
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

//Boilerplate all binary operations follow this format.
#define BINARY_OP(op) \
    do {\
        double b = pop(); \
        double a = pop(); \
        push(a op b); \
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
            case OP_ADD: {
                BINARY_OP(+);
                break;
            }
            case OP_SUBTRACT: {
                BINARY_OP(-);
                break;
            }
            case OP_MULTIPLY: {
                BINARY_OP(*);
                break;
            }
            case OP_DIVIDE: {
                BINARY_OP(/);
                break;
            }
            case OP_NEGATE: {
                // get the constant from the stack
                Value constant = pop();
                //negate it and push it back onto the stack
                push(-constant);
                break;
            }
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }

            //code
        }
    }

    //undefine the macros after were done with it
    #undef READ_CONSTANT
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


#include "commonlibs.h"
#include "chunk.h"
#include "debug.h"
#include "stdio.h"
#include "vm.h"


int main(int argc, char* argv[]) {
    initVM();

    Chunk chunk;
    initChunk(&chunk);

    //test value, get an index
    int constant = addConstant(&chunk, 1.2);
    //write a constant opcode to the bytecode stream, and write the index where that constant is in the value array
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    constant = addConstant(&chunk, 3.4);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_ADD, 123);


    constant = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

     writeChunk(&chunk, OP_DIVIDE, 123);

    writeChunk(&chunk, OP_NEGATE, 123);


    writeChunk(&chunk, OP_RETURN, 123);


    //printf("%d\n", sizeof(chunk));
    disassembleChunk(&chunk, "test chunk");

    interpret(&chunk);
    freeVM();
    
    freeChunk(&chunk);




    return 0;
}


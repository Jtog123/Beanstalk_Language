#include "commonlibs.h"
#include "chunk.h"
#include "debug.h"
#include "stdio.h"


int main(int argc, char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);

    //test value, get an index
    int constant = addConstant(&chunk, 1.2);
    //write a constant opcode to the bytecode stream, and write the index where that constant is in the value array
    writeChunk(&chunk, OP_CONSTANT);
    writeChunk(&chunk, constant);

    writeChunk(&chunk, OP_RETURN);
    printf("%d\n", sizeof(chunk));
    disassembleChunk(&chunk, "test chunk");

    

    freeChunk(&chunk);
    return 0;
}


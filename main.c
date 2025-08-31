#include "commonlibs.h"
#include "chunk.h"
#include "debug.h"
#include "stdio.h"


int main(int argc, char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);
    writeChunk(&chunk, OP_RETURN);
    printf("%d\n", sizeof(chunk));
    disassembleChunk(&chunk, "test chunk");

    

    freeChunk(&chunk);
    return 0;
}


#ifndef CHUNK_H
#define CHUNK_H

#include "commonlibs.h"
#include "memory.h"
#include "value.h"


/*
                            --------------------- OPCODE --------------------- 
What instruction are we dealing with?
*/
enum OpCode {
    OP_CONSTANT,
    OP_RETURN,
};



/*
                            --------------------- CHUNK ---------------------
                
Chunks store instructions, we will read from chunks to get their instructions

It will contain a count of the number of instructions.
It will contain a capacity, or the current maximum amount of instructions it can currently hold
It will contain an array (pointer) to uint8_t type, which will store contiguous blocks of our bytecode instructions ex. 00000001

Chunks will be dynamically doubled in size if the capacity < count + 1, functioning kind of like a vector in CPP.
*/


//Define an alias for the Struct
typedef struct {
    int count;
    int capacity;
    uint8_t* code;

    //store the constant values for each Chunk
    ValueArray constants;

} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte);
int addConstant(Chunk* chunk, Value value);
void freeChunk(Chunk* chunk);

#endif
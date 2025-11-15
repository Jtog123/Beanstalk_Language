
#include <stdlib.h>
#include "chunk.h"
#include "vm.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;

    //init the constants of the Chunk
    initValueArray(&chunk->constants);

}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if(chunk->capacity < (chunk->count + 1)) {
        // We are at capacity, get the old capacity
        int oldCapacity = chunk->capacity;

        //get a number for new capacity
        chunk->capacity = GROW_CAPACITY(oldCapacity);

        //Dynamically grow our array/chunk
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);

        // Grow line information
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
    }

    //append to end of code array, increment the number of instructions in the code
    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;


}

//writeConstant(Chunk, value, line)

int addConstant(Chunk* chunk, Value value) {
    push(value);//for GC
    writeValueArray(&chunk->constants, value);
    pop(); //for GC
    
    //return the index of the value we just added in the value array;
    return chunk->constants.count - 1;
}

void freeChunk(Chunk* chunk) {
    // passed in newSize as 0 which will releease the memory on the heap and returns NULL
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);

    //also free the value array
    freeValueArray(&chunk->constants);

    //zero out all of the memebers
    initChunk(chunk);
}
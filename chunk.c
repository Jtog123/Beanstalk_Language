
#include <stdlib.h>
#include "chunk.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

}

void writeChunk(Chunk* chunk, uint8_t byte) {
    if(chunk->capacity < (chunk->count + 1)) {
        // We are at capacity, get the old capacity
        int oldCapacity = chunk->capacity;

        //get a number for new capacity
        chunk->capacity = GROW_CAPACITY(oldCapacity);

        //Dynamically grow our array/chunk
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    //append to end of Chunk, increment the number of instructions in the chunk
    chunk->code[chunk->count] = byte;
    chunk->count++;


}

void freeChunk(Chunk* chunk) {
    // passed in newSize as 0 which will releease the memory on the heap and returns NULL
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);

    //zero out all of the memebers
    initChunk(chunk);
}
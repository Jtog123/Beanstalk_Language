#include "chunk.h"


Chunk::Chunk() : count(0), capacity(0), code(nullptr) {

}


void Chunk::writeChunk(uint8_t byte) {
    if(capacity < count + 1) {
        int oldCapacity = capacity;
        capacity = GROW_CAPACITY(oldCapacity);
        code = GROW_ARRAY(uint8_t, code, oldCapacity, capacity);
        
    }
    code[count] = byte;
    count++;

}

void Chunk::freeChunk() {
    FREE_ARRAY(uint8_t, code, capacity);
    count = 0;
    capacity = 0;
    code = nullptr;
}

Chunk::~Chunk() {

}

int Chunk::getChunkCount() {
    return count;
}

int Chunk::getChunkCapacity() {
    return capacity;
}

uint8_t* Chunk::getChunkCode() {
    return code;
}
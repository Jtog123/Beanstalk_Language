#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>

using namespace std;

/*
                            --------------------- CHUNK ---------------------
                
Chunks store instructions, we will read from chunks to get their instructions

It will contain a count of the number of instructions.
It will contain a capacity, or the current maximum amount of instructions it can currently hold
It will contain an array (pointer) to uint8_t type, which will store contiguous blocks of our bytecode instructions ex. 00000001

Chunks will be dynamically doubled in size if the capacity < count + 1, functioning pretty much like a standard vector.
In CPP I will probably opt for move operations over making copies, since moves are typically more efficient.
*/

class Chunk {

    public:
        Chunk(int count, int capacity, uint8_t* code);

};

#endif
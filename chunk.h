#ifndef CHUNK_H
#define CHUNK_H

#include "commonlibs.h"
#include "memory.h"

using namespace std;

/*
                            --------------------- OPCODE --------------------- 
What instruction are we dealing with?
*/


enum OpCode {
    OP_RETURN,
};



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
        
        Chunk();

        //Chunk(int count, int capacity, uint8_t* code);

        ~Chunk();

        // Write to a piece of bytecode, functions like a push_back
        void writeChunk(uint8_t byte);
        void freeChunk();

        int getChunkCount();
        int getChunkCapacity();
        uint8_t* getChunkCode();

    
    private:
    /* WE ARE ESSENTIALLY BUILDING A DYNAMIC ARRAY HOW CAN I CPPIFY THIS CODE??????*/
    
        //int GROW_CAPACITY(int oldCapacity);
        //void* GROW_ARRAY(decltype);
        //void* reallocate(void* pointer, size_t oldSize, size_t newSize);

    
    private:
        int count;
        int capacity;
        uint8_t* code;


};

#endif
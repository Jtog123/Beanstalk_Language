#include "chunk.h"
#include "debug.h"
#include "value.h"
#include "stdio.h"

//Takes in a piece of Code will fetch the instructions in the block
void disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);
    for(int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }

}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
    //Everytime we hit a constant insrtuction we also pull its constant index from the next (+ 1) byte in chunk/bytecode/instruction stream
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);

    //Chunk has an ValueArray of constants, constants is an array of values, we acces this array at the constant index
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    //constants are two byte instructions so we need to add 2 to iterate past.
    return offset + 2;



}

int disassembleInstruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);

    //get the instruction
    uint8_t instruction = chunk->code[offset];

    //switch on the instruction
    switch(instruction) {
        case OP_RETURN: 
            return simpleInstruction("OP_RETURN", offset);
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        default:
            printf("unknown opcode %d\n", instruction);
            return offset + 1;

    }

}


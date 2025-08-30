#include "debug.h"
#include <iostream>
#include <iomanip>

using namespace std;

void disassembleChunk(Chunk* chunk, const char* name) {
    cout << " == " << name << "  == " << endl;
    for(int offset = 0; offset < chunk->getChunkCount();) {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int simpleInstruction(const char* name, int offset) {
    cout << name << endl;
    return offset + 1;
}

int disassembleInstruction(Chunk* chunk, int offset) {
    cout << std::setfill('0') << std::setw(4)  << offset << " ";

    uint8_t instruction = chunk->getChunkCode()[offset];

    switch(instruction) {
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
        cout << "Unknown Opcode " << instruction << endl;
        return offset + 1;
    }
}


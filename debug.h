#ifndef BEANSTALK_DEBUG_H
#define BEANSTALK_DEBUG_H

#include "chunk.h"


void disassembleChunk(Chunk* chunk, const char* name);
int disassembleInstruction(Chunk* chunk, int offset);

#endif
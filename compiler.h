#ifndef BEANSTALK_COMPILER_H
#define BEANSTALK_COMPILER_H

#include "vm.h"

bool compile(const char* source, Chunk* chunk);

#endif
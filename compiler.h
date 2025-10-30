#ifndef BEANSTALK_COMPILER_H
#define BEANSTALK_COMPILER_H

#include "vm.h"
#include "object.h"

//bool compile(const char* source, Chunk* chunk);
ObjFunction* compile(const char* source);

#endif
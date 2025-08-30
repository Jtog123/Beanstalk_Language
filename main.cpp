#include <iostream>
#include "chunk.h"
#include "debug.h"

using namespace std;

int main(int argc, char* argv[]) {
    Chunk chunk;
    chunk.writeChunk(OP_RETURN);

    disassembleChunk(&chunk, "test chunk");
    chunk.freeChunk();
    cout << "wroking?" << endl;
    return 0;
}


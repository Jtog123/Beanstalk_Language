#include <stdio.h>

#include "commonlibs.h"
#include "compiler.h"
#include "scanner.h"

void compile(const char* source) {
    initScanner(source);
    int line = -1;

    for(;;) {
        //grab a token each time through the loop
        Token token = scanToken();
        if(token.line != line) {
            line = token.line;
        } else {
            printf("   |  ");
        }
        printf("%2d '%.*s\n", token.type, token.length, token.start);
        if(token.type == TOKEN_EOF) break;
    }

}
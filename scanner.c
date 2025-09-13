#include <stdio.h>
#include <string.h>

#include "commonlibs.h"
#include "scanner.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
    scanner.start = NULL;
    scanner.current = NULL;
    scanner.line = 1;
}
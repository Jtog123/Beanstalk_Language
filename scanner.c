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
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isAtEnd() {
    return *scanner.current == '\0';
}

static char advance() {
    //inc the pointer
    scanner.current++;
    //return the char we just moved past
    return scanner.current[-1];
}

static char peek() {
    return *scanner.current;
}

//FOR COMMENTS
static char peekNext() {
    if(isAtEnd()) return '\0';
    return scanner.current[1];
}

static bool match(char expected) {
    // if the expected token is correct return true else false
    if(isAtEnd()) return false;
    if(*scanner.current != expected) return false;

    scanner.current++;
    return true;


}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;

    return token;
}

static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}



static void skipWhitespace() {
    for(;;) {
        char c = peek();
        switch(c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '/':
                if(peekNext() == '/') {
                    //consume comments dont tokenize them
                    while(peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

//subtracting two pointers in C, the compiler gives the number of elemnts between them

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    //do some pointer arithmetic
    //mpmcmp compares the contents of bytes up to N bytes between first two args, if the content of the memory are equal we return 0
    if(scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {

    //DFA or State Machine, on encoutering a potential keyword, we search the rest of the string to validate

    switch(scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if(scanner.current - scanner.start > 1) { 
                printf("%s\n", "Printing the delta between characters");
                printf("%d\n", scanner.current - scanner.start);
                switch(scanner.start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'e': return checkKeyword(2, 1, "e", TOKEN_FEE_CLASS);
                    case 'i': return checkKeyword(2, 0, "", TOKEN_FI_IF); // compares 0 bytes
                    case 'o': return checkKeyword(2, 0, "", TOKEN_FO_FOR);
                    case 'u': return checkKeyword(2, 1, "m", TOKEN_FUM_FUN);
                }
            }
        case 'n': return checkKeyword(1,2, "il", TOKEN_NIL);
        case 'o': return checkKeyword(1,1, "r", TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 't': 
            if(scanner.current - scanner.start > 1) {
                switch(scanner.start[1]) {
                    case 'h': return checkKeyword(2,2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(2,2, "ue", TOKEN_TRUE);
                }
            }
        case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

//Looks from 2nd to n cahracters, first is read in scanToken
static Token identifier() {
    while(isAlpha(peek()) || isDigit(peek())) {
        advance();
    }

    return makeToken(identifierType());
}

static Token string() {
    while(peek() != '"' && !isAtEnd()) {
        if(peek() == '\n') {
            scanner.line++;
        }
        if(isAtEnd()) {
            return errorToken("ERROR: Unterminated String");
        }

    }

    //found the closing, push it forward
    advance();
    return makeToken(TOKEN_STRING);

}

static Token number() {
    while(isDigit(peek()))
        advance();
    
    //look for fracitonal part
    if(peek() == '.' && isDigit(peekNext())) {
        //consume the .
        advance();

        while(isDigit(peek())) advance();

    }

    return makeToken(TOKEN_NUMBER);
}


/*
static char advance() {
    //inc the pointer
    scanner.current++;
    //return the char we just moved past
    return scanner.current[-1];
}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}


*/




Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;

    if(isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();

    if(isDigit(c)) {
        return number();
    }

    if(isAlpha(c)) {
        return identifier();
    }

    switch(c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '!':
            return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(match('=') ? TOKEN_EQUAL_EQUAL: TOKEN_EQUAL);
        case '<':
            return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(match('=')? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"':
            return string();
    }

    return errorToken("Unexpected character.");
}
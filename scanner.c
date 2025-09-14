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



Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;

    if(isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();

    if(isDigit(c)) {
        return number();
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
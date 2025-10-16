#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commonlibs.h"
#include "compiler.h"
#include "scanner.h"

typedef struct {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

typedef enum {
    PREC_NONE, // LOW
    PREC_ASSIGNMENT, // =
    PREC_OR, // or
    PREC_AND, // and
    PREC_EQUALITY, // == !=
    PREC_COMPARISON,
    PREC_TERM, // + -
    PREC_FACTOR, // * /
    PREC_UNARY, // ! -
    PREC_CALL, // . ()
    PREC_PRIMARY //HIGH
} Precedence;

typedef void (*ParseFn)(bool canAssign);



typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

// compare TOKEN_IDENTIFIER lexeme to name, so we store it
// the scope depth of the block where the local was declared
typedef struct {
    Token name;
    int depth;
} Local;

typedef struct {
    Local locals[UINT8_COUNT];
    int localCount;
    int scopeDepth;
} Compiler;

Parser parser;
Compiler* current = NULL;
Chunk* compilingChunk;


///// Functions ///////

static Chunk* currentChunk() {
    return compilingChunk;
}

static void errorAt(Token* token, const char* message) {
    if(parser.panicMode) return;
    parser.panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);
    
    if(token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {

    } else {
        fprintf(stderr, "at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}


static void error(const char* message) {
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char* message) {
    errorAt(&parser.current, message);
}


static void advance() {
    // grab the token were about to move on from, this is why its primed in compile()
    parser.previous = parser.current;

    for(;;) {
        //send the token to the parser
        parser.current = scanToken();
        if(parser.current.type != TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);

    }
}

//consumes next token also valiadtes token type
static void consume(TokenType type, const char* message) {
    if(parser.current.type == type){
        advance();
        return;
    }
    errorAtCurrent(message);
}

static bool check(TokenType type) {
    return parser.current.type == type;
}

//cosume the token if the current token matches the one we pass in
static bool match(TokenType type) {
    if(!check(type)) return false;
    advance();
    return true;
}

//append a single byte to a chunk
static void emitByte(uint8_t byte) {
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn() {
    emitByte(OP_RETURN);
}

static uint8_t makeConstant(Value value) {
    //get the index in the values array
    int constant = addConstant(currentChunk(), value);
    if(constant > UINT8_MAX) { // can stoer up to 256 constants per chunk
        error("Too many constants in one chunk");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitConstant(Value value) {
    //dynamically making isntructions
    //first byte is opcode, second is index to value in value array
    emitBytes(OP_CONSTANT, makeConstant(value));
}

void initCompiler(Compiler* compiler) {
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    current = compiler;
}



static void endCompiler() {
    emitReturn();

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
    if(!parser.hadError) {
        disassembleChunk(currentChunk(), "code");
    }
#endif
}

static void beginScope() {
    current->scopeDepth++;
}

static void endScope() {
    current->scopeDepth--;

    //Poping scope
    while(current->localCount > 0 && current->locals[current->localCount - 1].depth > current->scopeDepth) {
        emitByte(OP_POP);
        current->localCount--;
    }
}




/*FORWARD DECLARATIONS*/
static void expression();
static void statement();
static void declaration();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);
static uint8_t identifierConstant(Token* name);
static int resolveLocal(Compiler* compiler, Token* name);
/*--------------------*/


static void binary(bool canAssign) {
    TokenType operatorType = parser.previous.type;
    ParseRule* rule = getRule(operatorType);
    //pass in the precedence and increase the precedence level + 1
    parsePrecedence((Precedence) (rule->precedence + 1));

    switch(operatorType) {
        case TOKEN_BANG_EQUAL: emitBytes(OP_EQUAL, OP_NOT); break;
        case TOKEN_EQUAL_EQUAL: emitByte(OP_EQUAL); break;
        case TOKEN_GREATER: emitByte(OP_GREATER); break;
        case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break; // logical equivalence check if less thn not the result
        case TOKEN_LESS: emitByte(OP_LESS); break;
        case TOKEN_LESS_EQUAL: emitBytes(OP_GREATER, OP_NOT); break;// logical equivalance check if greater than not the reuslt
        case TOKEN_PLUS : emitByte(OP_ADD); break;
        case TOKEN_MINUS: emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR: emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH: emitByte(OP_DIVIDE); break;
        default:
            return;
    }
}

static void literal(bool canAssign) {
    switch(parser.previous.type) {
        case TOKEN_FALSE: emitByte(OP_FALSE); break;
        case TOKEN_NIL: emitByte(OP_NIL); break;
        case TOKEN_TRUE: emitByte(OP_TRUE); break;
        default: return;
    }
}

//when we hit a '(' we know were looking for paranthsized grouping expression
static void grouping(bool canAssign) {
    expression();
    consume(TOKEN_RIGHT_PAREN, "expected ')' after expression");
}

//compiling number literals
static void number(bool canAssign) {
    double value = strtod(parser.previous.start, NULL);
    emitConstant(NUMBER_VAL(value));
}

//parser hits string token call string, +1 and -2 chop off leading and trailing quotes
//then creates Obj string, puts it in constant table (value array)
static void string(bool canAssign) {
    printf("DEBUG: String function called\n");
    emitConstant(OBJ_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}

static void namedVariable(Token name, bool canAssign) {
    uint8_t getOp, setOp;
    int arg = resolveLocal(current, &name);
    if(arg != -1) {
        //look for local
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    } else {
        //assume global
        arg = identifierConstant(&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if(canAssign && match(TOKEN_EQUAL)) {
        expression();
        //sends in the instruction along with the index so the vm get get that value on the stack
        // (OP_SET_LOCAL , 0)
        emitBytes(setOp, (uint8_t)arg);
    } else {
        emitBytes(getOp, (uint8_t)arg);
    }

}

static void variable(bool canAssign) {
    namedVariable(parser.previous, canAssign);
}

static void unary(bool canAssign) {
    TokenType operatorType = parser.previous.type;

    // operator consumed now compile the expression
    parsePrecedence(PREC_UNARY);

    switch(operatorType) {
        case TOKEN_BANG: emitByte(OP_NOT); break;
        case TOKEN_MINUS: emitByte(OP_NEGATE); break;
        default: return;
    }
}

// 1st col: function to compile prefix expressions starting with a token of that type
// 2nd col: function to compile infix expressions whose left operand is followed by a token of that type
// 3rd col : The precedence of the infix expression
ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {variable, NULL, PREC_NONE},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_FEE_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FO_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUM_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_FI_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE}


};


static void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

static void block() {
    //while we havent hit a right brace or EOF we are in a declaration of dsome kind
    while(!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
        declaration();
    }

    //expect to consunme this, else throw the error
    consume(TOKEN_RIGHT_BRACE, "Expect '}' after block");
}

//starts at current token and parses any expression at given precedence level or higher
static void parsePrecedence(Precedence precedence) {

    //Parsing Prefix Expressions
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if(prefixRule == NULL) {
        error("Expected expression");
        return;
    }

    bool canAssign = precedence <= PREC_ASSIGNMENT;
    prefixRule(canAssign);

    //looking for potential infix operator
    while(precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;

        //pointer to a fnuction
        infixRule(canAssign);
    }

    if(canAssign && match(TOKEN_EQUAL)) {
        error("invalid assignment target");
    }

}

static uint8_t identifierConstant(Token* name) {
    // ex var bacon = 5;
    // sends 'bacon' to chunks constant table as string (HASH map)
    //it then returns the index of where its located in the constant table
    return makeConstant(OBJ_VAL(copyString(name->start, name->length)));
}

static bool identifiersEqual(Token* a, Token* b) {
    if(a->length != b->length) return false;
    //check if the contents of each string are the same
    return memcmp(a->start, b->start, a->length) == 0;
}

static int resolveLocal(Compiler* compiler, Token* name) {
    for(int i = compiler->localCount - 1; i >= 0; i--) {
        Local* local = &compiler->locals[i];
        if(identifiersEqual(name, &local->name)) {
            if(local->depth == -1) {
                error("Cant read local variable in its own initializer");
            }
            return i;
        }
    }

    return -1;
}

static void addLocal(Token name) {

    if(current->localCount == UINT8_COUNT) {
        error("Too many local variables in function.");
        return;
    }

    //add it to array
    Local* local = &current->locals[current->localCount++];
    local->name = name;
    local->depth = -1;
}

static void declareVariable() {
    //if we have global scope return
    if(current->scopeDepth == 0) return;

    Token* name = &parser.previous;

    for(int i = current->localCount - 1; i >= 0; i--) {
        Local* local = &current->locals[i];
        if(local->depth != -1 && local->depth < current->scopeDepth) {
            break;
        }

        //two locals with same name in same scope
        if(identifiersEqual(name, &local->name)) {
            error("Already a variable with this name in this scope.");
        }
    }

    //pass in token to add to local scope
    addLocal(*name);

}


static uint8_t parseVariable(const char* errorMessage) {
    //consume the name of thje variable, 'bacon'
    consume(TOKEN_IDENTIFIER, errorMessage);

    declareVariable();

    //if we have a local reuturn
    if(current->scopeDepth > 0)
        return 0;

    //grab the string
    return identifierConstant(&parser.previous);
}

static void markInitialized() {
    current->locals[current->localCount - 1].depth = current->scopeDepth;
}

static void defineVariable(uint8_t global) {

    //if local dont need to emit
    if(current->scopeDepth > 0) {
        markInitialized();
        return;
    }

    //emit the global op code and the index to the current chunk
    emitBytes(OP_DEFINE_GLOBAL, global);
}

static void expressionStatement() {
    expression(); // start parsing
    consume(TOKEN_SEMICOLON, "Expect ';' after expression");
    emitByte(OP_POP);
}

static void printStatement() {
    expression();
    //check if next token is semicolon if it is consume it move pointer forward, if not throw error
    consume(TOKEN_SEMICOLON, "Expected ';' after value");
    emitByte(OP_PRINT);
}

static void synchronize() {
    parser.panicMode = false;

    while(parser.current.type != TOKEN_EOF) {
        if(parser.previous.type == TOKEN_SEMICOLON) return;

        switch(parser.current.type) {
            case TOKEN_FEE_CLASS:
            case TOKEN_FUM_FUN:
            case TOKEN_VAR:
            case TOKEN_FO_FOR:
            case TOKEN_FI_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
                return;

            default:
            ; // do nothing
        }

        advance();
    }
}

static void varDeclaration() {
    //parseVariable() -> identifierConstant(), gets returned an index of the constant in the hash map
    uint8_t global = parseVariable("Expect a variable name.");

    //if variable has a value assign it, or give it a Nil value
    if(match(TOKEN_EQUAL)) {
        expression();
    } else {
        emitByte(OP_NIL);
    }
    //both statements will have a semicolon, so consume
    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration");

    defineVariable(global);
}



//if our current token is a var we have a declaration, else we have a statement
static void declaration() {
    if(match(TOKEN_VAR)) {
        varDeclaration();
    } else {
        statement();
    }

    //if we hit a compile error while parsing previous statement
    if(parser.panicMode) synchronize();
}

static void statement() {
    if(match(TOKEN_PRINT)) {
        printStatement();
    } else if(match(TOKEN_LEFT_BRACE)) {
        beginScope();
        block();
        endScope();
    }
    else {
        expressionStatement();
    }
}









// look up the rules for the operator
static ParseRule* getRule(TokenType type) {
    return &rules[type];
}





//passing the chunk to write to
bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    Compiler compiler;
    initCompiler(&compiler);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    //primer
    advance();

    //Beanstalk is a sequence of declarations
    while(!match(TOKEN_EOF)) {
        declaration();
    }
    endCompiler();
    return !parser.hadError;

}
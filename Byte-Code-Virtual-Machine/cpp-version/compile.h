#ifndef clox_compiler_h
#define clox_compiler_h
#include<functional>
#include <memory>
#include <string>
#include"chunk.h"
#include "Token.h"
#include "scanner.h"
#include "object.h"
#include "common.h"

typedef struct {
  Token current;
  Token previous;
    bool hadError;
      bool panicMode;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
} Precedence;

 // typedef returnType(*functionPointer)(arguments);

using ParseFn = std::function<void(bool)>;



typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

typedef struct {
  Token name;
  int depth;
} Local;

typedef struct {
  Local locals[UINT8_COUNT];
  int localCount;
  int scopeDepth;
} _Compiler;

class Compiler {
  std::string source;
  Parser parser;
_Compiler* current = NULL;
  Scanner scanner;
   void advance();
    void  errorAtCurrent(const std::string &message);
    void errorAt(Token* token, const std::string &message);
    void   consume(TokenType type, std::string message) ;
    void error(const std::string  &message) ;
     void emitReturn() ;
     Chunk* currentChunk() ;
      void  endCompiler();
      void  emitBytes(uint8_t byte1, uint8_t byte2);
    void  emitByte(uint8_t byte);
    void expression();
     void  number(bool canAssign);
     void emitConstant(Value value);
      uint8_t  makeConstant(Value value);
      void grouping(bool canAssign);
      void unary(bool canAssign);
      void parsePrecedence(Precedence precedence);
      void binary(bool canAssign);
       void  literal(bool canAssign) ;
        void statement();
       void declaration();
       bool match(TokenType type);
        bool check(TokenType type) ;
         void printStatement();
         void expressionStatement();
         void synchronize();
       void varDeclaration();
        uint8_t parseVariable(const char* errorMessage) ;
        uint8_t identifierConstant(Token* name) ;
        void  defineVariable(uint8_t global);
       void string(bool canAssign);
       ParseRule* getRule(TokenType type) ;
        void  namedVariable(Token name,bool canAssign);
         void variable(bool canAssign) ;
 void initCompiler(_Compiler* compiler);
       ParseRule rules[static_cast<int>(TokenType::TOKEN_COUNT)];
       void initRules();
        void block();
        void beginScope();
         void endScope();
         void declareVariable() ;
          void addLocal(Token name);
          bool identifiersEqual(Token* a, Token* b);
           int resolveLocal(_Compiler* compiler, Token* name);
           void markInitialized() ;
           void ifStatement() ;
           int emitJump(uint8_t instruction);
            void   patchJump(int offset);
             void and_(bool canAssign);
              void or_(bool canAssign);
               void whileStatement();
                void emitLoop(int loopStart);
                 void forStatement();
public:
  Compiler(const std::string& src);


  bool compile(Chunk* chunk);

};

#endif

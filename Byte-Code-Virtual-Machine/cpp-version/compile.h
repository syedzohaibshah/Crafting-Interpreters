#ifndef clox_compiler_h
#define clox_compiler_h
#include<functional>
#include <memory>
#include <string>
#include"chunk.h"
#include "Token.h"
#include "scanner.h"


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

using ParseFn =std::function<void()>;  
 


typedef struct {
  ParseFn prefix; 
  ParseFn infix;
  Precedence precedence;
} ParseRule;


class Compiler {
  std::string source;
  Parser parser;

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
     void  number();
     void emitConstant(Value value);
      uint8_t  makeConstant(Value value);
      void grouping();
      void unary();
      void parsePrecedence(Precedence precedence);
      void binary();
       ParseRule* getRule(TokenType type) ;

       ParseRule rules[static_cast<int>(TokenType::TOKEN_COUNT)];
       void initRules();
public:
  Compiler(const std::string& src);
 
  
  bool compile(Chunk* chunk);

};

#endif
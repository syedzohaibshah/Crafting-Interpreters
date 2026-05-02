#ifndef clox_compiler_h
#define clox_compiler_h

#include <memory>
#include <string>
#include"chunk.h"
#include "Token.h"
#include "scanner.h"

class Compiler {
  std::string source;
  Scanner scanner;
   void advance();
    void  errorAtCurrent(const std::string &message);
    void errorAt(Token* token, const std::string &message);
    void   consume(TokenType type, std::string message) ;
    void error(const std::string  &message) ;
    
public:
  Compiler(const std::string& src);
 
  
  bool compile(Chunk* chunk);

};

#endif
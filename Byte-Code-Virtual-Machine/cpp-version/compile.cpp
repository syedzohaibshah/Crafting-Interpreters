#include <stdio.h>

#include "common.h"
#include "compile.h"
#include "scanner.h"
#include <string>
#include <string_view>
#include <iomanip>
#include <string_view>
#include<iostream>

typedef struct {
  Token current;
  Token previous;
    bool hadError;
      bool panicMode;
} Parser;

Parser parser;

void Compiler:: errorAt(Token* token, const std::string & message) {
    if(parser.panicMode) return;
    parser.panicMode=true;
  std::cout<< "[line "<<token->line<<"] Error \n";

  if (token->type == TokenType::TOKEN_EOF) {
    std::cout<<" at end";
  } else if (token->type == TokenType::TOKEN_ERROR) {
    // Nothing.
  } else {

      std::string_view lexeme(source.data() + token->start, token->length);
           std::cout<<"at '"<<lexeme<<"'"<<std::endl;
    //fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

 std::cout<<":"<< message<<std::endl;
  parser.hadError = true;
}

void Compiler:: error(const std::string & message) {
  errorAt(&parser.previous, message);
}

 void Compiler:: errorAtCurrent(const std::string & message) {
  errorAt(&parser.current, message);
}

Compiler::Compiler(const std::string& src) : source(src),scanner(src) {}



 void Compiler::advance() {
     
  parser.previous = parser.current;

  for (;;) {
    parser.current = scanner.scanToken();
    if (parser.current.type != TokenType::TOKEN_ERROR) break;
errorAtCurrent("Unexpected character");
   
  }
}

 void  Compiler:: consume(TokenType type, std::string message) {
  if (parser.current.type == type) {
    advance();
    return;
  }

  errorAtCurrent(message);
}

bool Compiler::compile(Chunk* chunk) {
 
  parser.hadError = false;
  parser.panicMode = false;
  advance();
  expression();
  consume(TokenType::TOKEN_EOF, "Expect end of expression.");

   return !parser.hadError;
 
}







// int line = -1;              
//   for (;;) {
//     Token token = scanner.scanToken();
//     if (token.line != line) {
//       std::cout<<std::setw(4)<<token.line<<" ";
//       line = token.line;
//     } else {
        
//       std::cout << "   | ";
//     }
//     std::string_view lexeme(source.data() + token.start, token.length);
//     std::cout<<std::setw(2)<<static_cast<int>(token.type)<<" '"<<lexeme<<"'"<<std::endl;
//     if (token.type == TokenType::TOKEN_EOF) break;
//   }
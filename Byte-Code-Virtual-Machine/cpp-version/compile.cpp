#include <stdio.h>

#include "Token.h"
#include "common.h"
#include "compile.h"
#include "scanner.h"
#include <string>
#include <string_view>
#include <iomanip>
#include <string_view>
#include<iostream>
#include "debug.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

Chunk* compilingChunk;


Compiler::Compiler(const std::string& src) : source(src),scanner(src) {
     initRules();
}


Chunk* Compiler:: currentChunk() {
  return compilingChunk;
}

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

void Compiler:: emitByte(uint8_t byte) {
  compilingChunk->writeChunk(byte, parser.previous.line);
}
void Compiler:: emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}
 void Compiler:: emitReturn() {
  emitByte(OP_RETURN);
}
 void Compiler::  endCompiler() {
  emitReturn();
  #ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
      disassembleChunk(*currentChunk(), "code");
    }
  #endif



}

void Compiler:: parsePrecedence(Precedence precedence) {

    advance();// we are now on the second token
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;    /*getRule will Return pointer to the token type function
        which is actalyy type of ParserRule whic is actually struct in that struct we have three column( types
        of functions prefix and infix and also precedence of these operations)   */

        if (prefixRule == nullptr) {
      error("Expect expression.");
      return;
    }

    prefixRule(); //it will call that function that we just get from table

    while (precedence <= getRule(parser.current.type)->precedence) {
      advance();
      ParseFn infixRule = getRule(parser.previous.type)->infix;
      infixRule();
    }

}

 ParseRule* Compiler::getRule(TokenType type) {
  return &rules[static_cast<int>(type)]; //gives pointer to the type of token
}

void Compiler::expression() {
  // What goes here?
   parsePrecedence(PREC_ASSIGNMENT);
}

void Compiler:: unary() {
  TokenType operatorType = parser.previous.type;

  // Compile the operand.
  //expression();
parsePrecedence(PREC_UNARY);
  // Emit the operator instruction.
  switch (operatorType) {
    case TokenType::TOKEN_BANG: emitByte(OP_NOT); break;
    case TokenType::TOKEN_MINUS: emitByte(OP_NEGATE); break;
    default: return; // Unreachable.
  }
}

void Compiler:: grouping() {
  expression();
  consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}


 uint8_t Compiler:: makeConstant(Value value) {
  int constant = compilingChunk->addConstant( value);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0;
  }

  return (uint8_t)constant;
}
 void  Compiler:: emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

 void Compiler:: number() {

    std::string_view val(source.data() + parser.previous.start, parser.previous.length);
    std::string strValue=static_cast<std::string>(val);
    double value = stod(strValue);
    //emitConstant(value);
    emitConstant(NUMBER_VAL(value));

}

 void Compiler:: binary() {
  TokenType operatorType = parser.previous.type;
  ParseRule* rule = getRule(operatorType);
  parsePrecedence((Precedence)(rule->precedence + 1));

  switch (operatorType) {
    case TokenType::TOKEN_PLUS:          emitByte(OP_ADD); break;
    case TokenType::TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
    case TokenType::TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
    case TokenType::TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
    case TokenType::TOKEN_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
    case TokenType::TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
    case TokenType::TOKEN_GREATER:       emitByte(OP_GREATER); break;
    case TokenType::TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
    case TokenType::TOKEN_LESS:          emitByte(OP_LESS); break;
    case TokenType::TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
    default: return; // Unreachable.
  }
}

 void Compiler:: literal() {
  switch (parser.previous.type) {
    case TokenType::TOKEN_FALSE: emitByte(OP_FALSE); break;
    case TokenType::TOKEN_NIL: emitByte(OP_NIL); break;
    case TokenType::TOKEN_TRUE: emitByte(OP_TRUE); break;
    default: return; // Unreachable.
  }
}

 void Compiler:: string() {
  emitConstant(OBJ_VAL(copyString(source.data() + parser.previous.start + 1,
                                  parser.previous.length - 2)));
}

bool Compiler::compile(Chunk* chunk) {
 compilingChunk = chunk;
  parser.hadError = false;
  parser.panicMode = false;
  advance();
  expression();
  consume(TokenType::TOKEN_EOF, "Expect end of expression.");
 endCompiler();
   return !parser.hadError;

}



void Compiler::initRules() {

  rules[static_cast<int>(TokenType::TOKEN_LEFT_PAREN)] = {[this]() { this->grouping();}, nullptr, PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_RIGHT_PAREN)]   = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_LEFT_BRACE)]   = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_RIGHT_BRACE)]   = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_COMMA)]         = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_DOT)]           = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_MINUS)]         = {[this]() { this->unary();},[this]() { this->binary();}, PREC_TERM};
  rules[static_cast<int>(TokenType::TOKEN_PLUS)]          = {nullptr,     [this]() { this->binary();}, PREC_TERM};
  rules[static_cast<int>(TokenType::TOKEN_SEMICOLON)]     = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_SLASH)]         = {nullptr,     [this]() { this->binary();}, PREC_FACTOR};
  rules[static_cast<int>(TokenType::TOKEN_STAR)]          = {nullptr,     [this]() { this->binary();}, PREC_FACTOR};
  rules[static_cast<int>(TokenType::TOKEN_BANG)]          = {[this](){this->unary();}, nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_BANG_EQUAL)]    = {nullptr,     [this](){this->binary();},   PREC_EQUALITY};//
  rules[static_cast<int>(TokenType::TOKEN_EQUAL)]         = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_EQUAL_EQUAL)]   = {nullptr,     [this](){this->binary();},   PREC_EQUALITY};//
  rules[static_cast<int>(TokenType::TOKEN_GREATER)]       = {nullptr,     [this](){this->binary();},   PREC_COMPARISON};
  rules[static_cast<int>(TokenType::TOKEN_GREATER_EQUAL)] = {nullptr,     [this](){this->binary();},   PREC_COMPARISON};
  rules[static_cast<int>(TokenType::TOKEN_LESS)]          = {nullptr,     [this](){this->binary();},   PREC_COMPARISON};
  rules[static_cast<int>(TokenType::TOKEN_LESS_EQUAL)]    = {nullptr,     [this](){this->binary();},   PREC_COMPARISON};
  rules[static_cast<int>(TokenType::TOKEN_IDENTIFIER)]    = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_STRING)]        = {[this](){this->string();},  nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_NUMBER)]        = {[this]() { this->number();},   nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_AND)]           = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_CLASS)]         = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_ELSE)]          = {nullptr,     nullptr,   PREC_NONE}; ///
  rules[static_cast<int>(TokenType::TOKEN_FALSE)]         = {[this](){this->literal();},  nullptr,   PREC_NONE};///
  rules[static_cast<int>(TokenType::TOKEN_FOR)]           = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_FUN)]           = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_IF)]            = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_NIL)]           = {[this](){this->literal();},  nullptr,   PREC_NONE};//
  rules[static_cast<int>(TokenType::TOKEN_OR)]            = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_PRINT)]         = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_RETURN)]        = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_SUPER)]         = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_THIS)]          = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_TRUE)]          = {[this](){this->literal();}, nullptr,   PREC_NONE};//
  rules[static_cast<int>(TokenType::TOKEN_VAR)]           = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_WHILE)]         = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_ERROR)]         = {nullptr,     nullptr,   PREC_NONE};
  rules[static_cast<int>(TokenType::TOKEN_EOF)]           = {nullptr,     nullptr,   PREC_NONE};
};





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

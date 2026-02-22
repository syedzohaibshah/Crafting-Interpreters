
#include "Parser.h"
#include <initializer_list>

#include "Lox.h"
std::unique_ptr<Expr> Parser::expression(){

     return equality();
 }

std::unique_ptr<Expr> Parser::equality(){
    auto expr=comparison();

    while(match({BANG_EQUAL,EQUAL_EQUAL})){
        Token op=previous();
        auto right=comparison();

        expr = std::make_unique<Binary>(
            std::move(expr),
            op,
            std::move(right));

    }
    return expr;

}


// comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;

std::unique_ptr<Expr> Parser::comparison(){


    auto  expr=term();
    while(match({GREATER,GREATER_EQUAL,LESS,LESS_EQUAL})){
        Token op=previous();
        auto right=term();
        expr=std::make_unique<Binary>(
            std::move(expr),
            op,
            std::move(right));

    }

    return expr;
}


std::unique_ptr<Expr> Parser:: term() {
  auto  expr = factor();

  while (match({MINUS, PLUS})) {
    Token op = previous();
    auto right = factor();
    expr = std::make_unique<Binary>(
        std::move(expr),
        op,
        std::move(right));

  }

  return expr;
}


std::unique_ptr<Expr> Parser::factor() {
  auto expr = unary();

  while (match({SLASH, STAR})) {
    Token op = previous();
    auto right = unary();
    expr = std::make_unique<Binary>(
        std::move(expr),
        op,
        std::move(right));

  }

  return expr;
}


std::unique_ptr<Expr> Parser::  unary() {

    if (match({BANG, MINUS})) {
      Token op= previous();
      auto right = unary();
      return std::make_unique<Unary>(op,std::move(right));

    }

    return primary();
  }

std::unique_ptr<Expr> Parser:: primary(){

    if(match({FALSE})) return std::make_unique<Literal>(false);
    if(match({TRUE})) return std::make_unique<Literal>(true);
    if(match({NIL})) return std::make_unique<Literal>(nullptr);


    if(match({NUMBER,STRING})){
        return std::make_unique<Literal>(previous().literal);
    }

    if(match({LEFT_PAREN})){
        auto expr=expression();
        consume(RIGHT_PAREN,"Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));


    }

throw error(peek(), "Expect expression.");

}


bool Parser::match(std::initializer_list<TokenType> types){


    for (TokenType type:types){

        if(check(type)){

            advance();
            return true;
        }
    }

    return false;
}


bool Parser::check(TokenType type){

    if(is_at_end()) return false;
    return peek().type==type;

}

Token Parser:: advance() {
   if (!is_at_end()) current++;
   return previous();
 }

bool Parser:: is_at_end() {
   return peek().type == END_OF_FILE;
 }

Token Parser::peek() {

   return tokens[current];
 }

Token Parser:: previous() {

   return tokens[current - 1];
 }
 Token Parser :: consume(TokenType type,std::string message){

     if(check(type)) return advance();

     throw error(peek(),message);

 }


  ParseError Parser::error(const Token& token,const std::string &message){

      Lox::error(token,message);
      return ParseError();

  }

void Parser::synchronize() {
    advance();

    while (!is_at_end()) {
      if (previous().type == SEMICOLON) return;

      switch (peek().type) {
        case CLASS:
        case FUN:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case PRINT:
        case RETURN:
          return;
      }

      advance();
    }
  }


  std::unique_ptr<Expr> Parser::parse(){


      try{
          return Parser::expression();
      }catch(ParseError &){

        Parser::synchronize();
        return nullptr;
      }
  }

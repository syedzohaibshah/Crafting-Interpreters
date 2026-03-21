#pragma once

#include<vector>
#include "Expr.h"
#include "TokenType.h"
#include "Token.h"
#include <memory>
#include <initializer_list>
#include "Lox.h"
#include "Stmt.h"


class ParseError : public std::runtime_error {
public:
    ParseError() : std::runtime_error("Parse error") {}
};

class Parser{

    std::vector<Token> tokens;
    int current=0;

   //helpers
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    Token advance();
    bool is_at_end();
    Token peek();
    Token previous();
    Token consume(TokenType type ,std::string message);
    void synchronize();

//Stmt
std::unique_ptr<Stmt> varDeclaration();
std::unique_ptr<Stmt>  declaration();
std::unique_ptr<Stmt> statement();
std::unique_ptr<Stmt> print_statement();
std::unique_ptr<Stmt> expression_statement();

std::vector<std::unique_ptr<Stmt>> block();

 //Expr
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr>  unary();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr>  term();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> expression();
 
   
    // these  are right associative all others are left
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> conditional();
    
    std::unique_ptr<Expr> assignment();

    
     ParseError error(const Token & token,const std::string &message);
     
    public:
    Parser(std::vector<Token> &tokens):tokens(tokens){}
std::vector<std::unique_ptr<Stmt>> parse();

};

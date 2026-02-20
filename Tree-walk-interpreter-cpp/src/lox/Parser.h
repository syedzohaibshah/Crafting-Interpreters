#pragma once

#include<vector>
#include "Expr.h"
#include "TokenType.h"
#include "Token.h"
#include <memory>
#include <initializer_list>

class Parser{

    std::vector<Token> tokens;
    int current=0;


    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    Token advance();
    bool is_at_end();
    Token peek();
    Token previous();

    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr>  unary();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr>  term();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();

    public:
    Parser(std::vector<Token> &tokens):tokens(tokens){}


};

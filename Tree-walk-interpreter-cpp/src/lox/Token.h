
#pragma once

#include<string>
#include<variant>
#include<utility>
#include<ostream>
#include"TokenType.h"
#include "Object.h"
using enum TokenType;


class Token{
public:
    const TokenType type;
    public: const std::string lexeme;
    const Object literal;
    const int line;


    Token(TokenType type,
        std::string lexeme,
        Object literal,
        int line)
        :type(type),
        lexeme(std::move(lexeme)),
        literal(std::move(literal)),
        line(line){}

std::string literal_to_string(const Object&value) const;
    std::string toString () const;
    friend std::ostream& operator<<(std::ostream& os, const Token& token);

};

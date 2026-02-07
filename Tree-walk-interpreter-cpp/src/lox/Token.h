
#pragma once

#include<string>
#include<variant>
#include<utility>
#include<ostream>
#include"TokenType.h"

using enum TokenType;

using Object= std::variant<std::monostate,double,std::string,bool>;  //(equivalent to java object) can hold one value from a fixed set of types, but only one at a time.

class Token{

    const TokenType type;
    const std::string lexeme;
    const Object literal;
    const int line;

    public:
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

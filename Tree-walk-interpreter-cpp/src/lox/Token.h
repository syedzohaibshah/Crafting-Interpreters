
#pragma once

#include<string>
#include<variant>
#include<utility>

enum class token_type;

using Object= std::variant<std::monostate,double,std::string,bool>;  //(equivalent to java object) can hold one value from a fixed set of types, but only one at a time.

class Token{

    const token_type type;
    const std:string lexeme;
    const Object literal;
    const int line;

    public:
    Token(token_type type,
        std::string lexeme,
        Object literal,
        int line)
        :type(type),
        lexeme(std::move(lexeme)),
        literal(std::move(literal)),
        line(line){}

std::string ;iteral_to_string(const Object&value);
    std::string toString () const;

};

#include"Token.h"
#include<string>
#include <variant>

#include"TokenType.h"

std::string Token::toString() const{
    return tokenTypeToString(type)+" "+lexeme+" "+literal_to_string(literal);
}


std::string Token::literal_to_string(const Object &value) const{

    if(std::holds_alternative<std::monostate>(value))
        return "nil";
    if(std::holds_alternative<double>(value))
        return std::to_string(std::get<double>(value));
    if(std::holds_alternative<std::string>(value))
        return std::get<std::string>(value);
    if(std::holds_alternative<bool>(value))
        return std::get<bool>(value)?"true":"false";


    return "";
}


std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << token.toString();
    return os;
}

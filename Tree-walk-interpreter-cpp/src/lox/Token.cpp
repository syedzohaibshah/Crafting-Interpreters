#include"Token.h"
#include<string>
#include <variant>



std::string Token::toString() const{

    return std::to_string(static_cast<int>(type))+" "+
    lexeme+" "+literal_to_string(literal);

}

std::string literal_to_string(const Object &value){

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

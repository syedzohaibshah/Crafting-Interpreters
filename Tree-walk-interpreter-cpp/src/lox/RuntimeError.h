#pragma once
#include<stdexcept>
#include"Token.h"

class RuntimeError:public std::runtime_error{
    public:
    const Token token;
    
    
    RuntimeError(Token token,std::string message)
                    :std::runtime_error(message),token(token){}
    

};
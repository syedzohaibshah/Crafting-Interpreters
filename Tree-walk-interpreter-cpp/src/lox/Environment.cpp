#include "Environment.h";



void Environment::define(std::string name,Object value){
    values[name]=value;
}
Object Environment::get(const Token &name){
    
    if(values.find(name.lexeme)!=values.end()){
        return values[name.lexeme];
    }
    
    throw RuntimeError(name,
           "Undefined variable '" + name.lexeme + "'.");
}


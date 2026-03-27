#include "Environment.h"



void Environment::define(std::string name,Object value){
    values[name]=value;
}

Object Environment::get(const Token &name){

    if(values.find(name.lexeme)!=values.end()){
        Object val=values[name.lexeme];
        if(std::holds_alternative<std::monostate>(val)){
            throw RuntimeError(name,
                   "variable is not intialized" + name.lexeme + "'.");
        }
        return val;
    }
    if(enclosing!=nullptr){
        return enclosing->get(name);
    }
    throw RuntimeError(name,
           "Undefined variable '" + name.lexeme + "'.");

}


void  Environment::assign(const Token name, const Object &value) {

  if (values.find(name.lexeme)!=values.end()) {
    values[name.lexeme] = value;
    return;
  }
if(enclosing!=nullptr){
    enclosing->assign(name,value);
    return;
}

  throw RuntimeError(name,
      "Undefined variable '" + name.lexeme + "'.");
}

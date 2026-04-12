#include "Environment.h"
#include <memory>



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


Object Environment:: getAt(int distance, std::string name) {
  return ancestor(distance)->values[name];
}


  std::shared_ptr<Environment>  Environment:: ancestor(int distance) {
    std::shared_ptr<Environment>  environment = std::make_shared<Environment>(*this);
    for (int i = 0; i < distance; i++) {
      environment = environment->enclosing;
    }

    return environment;
  }


  void Environment:: assignAt(int distance, Token name, Object value) {

    ancestor(distance)->values[name.lexeme]= value;
  }

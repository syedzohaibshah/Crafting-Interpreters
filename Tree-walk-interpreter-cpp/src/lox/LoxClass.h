

#pragma once

class Object;
#include "LoxCallable.h"

class LoxClass:public LoxCallable {
  const std::string name;
public:
  LoxClass(const std::string name):name(name){}

std::string toString();
  int arity() ;
  
  Object call(Interpreter *interpreter, 
      vector<Object> &arguments);

};

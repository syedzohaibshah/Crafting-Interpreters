

#pragma once

#include "Object.h"
#include "LoxCallable.h"

#include <vector>

class LoxClass: public LoxCallable {
  public:
  const std::string name;

  LoxClass(const std::string name):name(name){}

std::string toString()const override;
  int arity()override ;

  Object call(Interpreter *interpreter,
      const std::vector<Object> &arguments)override;

};

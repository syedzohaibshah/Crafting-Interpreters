

#pragma once

#include "LoxFunction.h"
#include "Object.h"
#include "LoxCallable.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


class LoxInstance;

class LoxClass: public LoxCallable,public std::enable_shared_from_this<LoxClass> {
  public:
  const std::string name;
std::unordered_map<std::string, std::shared_ptr<LoxFunction>>methods;
std::shared_ptr<LoxClass> metaclass;
  LoxClass(const std::string name,
      std::unordered_map<std::string,std::shared_ptr<LoxFunction>>methods,
      std::shared_ptr<LoxClass> metaclass)
  :name(name),methods(methods), metaclass(metaclass){}

std::string toString()const override;
  int arity()override ;

  Object call(Interpreter *interpreter,
      const std::vector<Object> &arguments)override;
   std::shared_ptr<LoxFunction> findMethod(std::string name);

};

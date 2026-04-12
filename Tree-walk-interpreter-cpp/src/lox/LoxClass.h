

#pragma once

#include "LoxFunction.h"
#include "Object.h"
#include "LoxCallable.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class LoxClass: public LoxCallable,public std::enable_shared_from_this<LoxClass> {
  public:
  const std::string name;
std::unordered_map<std::string, std::shared_ptr<LoxFunction>>methods;
  LoxClass(const std::string name,std::unordered_map<std::string,
      std::shared_ptr<LoxFunction>>methods)
  :name(name),methods(methods){}

std::string toString()const override;
  int arity()override ;

  Object call(Interpreter *interpreter,
      const std::vector<Object> &arguments)override;
   std::shared_ptr<LoxFunction> findMethod(std::string name);

};

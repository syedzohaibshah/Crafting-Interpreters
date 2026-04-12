#pragma once

#include "LoxClass.h"
#include <unordered_map>

#include "LoxFunction.h"
#include "Object.h"
#include "Token.h"
#include "RuntimeError.h"
class LoxInstance :public std::enable_shared_from_this<LoxInstance>{


public:
 std::shared_ptr<LoxClass> klass;
 std::unordered_map<std::string, Object>fields;
 explicit LoxInstance(std::shared_ptr<LoxClass>klass):klass(klass){}

std::string toString() {
    return klass->name + " instance";
  }

  Object get(Token name) {

    if (fields.find(name.lexeme)!=fields.end()) {
      return fields[name.lexeme];
    }
   // if cant find data member find method with same name
    auto method=klass->findMethod(name.lexeme);
    if(method!=nullptr) return method->bind(shared_from_this());  //see

    throw RuntimeError(name,
        "Undefined property '" + name.lexeme + "'.");
  }

  void set(Token name, Object value) {

    fields[name.lexeme]= value;
  }

};

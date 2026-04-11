#include"LoxClass.h"

#include "Object.h"



#include "LoxInstance.h"

#include "LoxCallable.h"




std::string LoxClass::toString()const {
    return name;
  }


  Object LoxClass:: call(Interpreter* interpreter,
      const std::vector<Object> &arguments) {
          
    auto cls = std::make_shared<LoxClass>(*this);
    return std::make_shared<LoxInstance>(cls);
  }


  int LoxClass::arity() {
    return 0;
  }

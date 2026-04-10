#include"LoxClass.h"

#include "Object.h"










std::string LoxClass::toString() {
    return name;
  }


  Object call(Interpreter* interpreter,
      vector<Object> &arguments) {
          
    LoxInstance instance(this);
    return instance;
  }


  int arity() {
    return 0;
  }

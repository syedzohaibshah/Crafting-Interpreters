#include"LoxClass.h"

#include "LoxFunction.h"
#include "Object.h"



#include "LoxInstance.h"

#include "LoxCallable.h"
#include <memory>




std::string LoxClass::toString()const {
    return name;
  }


  Object LoxClass:: call(std::shared_ptr<Interpreter> interpreter,
      const std::vector<Object> &arguments) {

    auto instance = std::make_shared<LoxInstance>(shared_from_this());

    std::shared_ptr<LoxFunction> initializer = findMethod("init");

    if (initializer != nullptr) {
      initializer->bind(instance)->call(interpreter, arguments);
    }


    return instance;
  }


  int LoxClass::arity() {
        std::shared_ptr<LoxFunction> initializer = findMethod("init");
        if (initializer == nullptr) return 0;
        return initializer->arity();

  }


 std::shared_ptr<LoxFunction> LoxClass:: findMethod(std::string name){

      if(methods.find(name)!=methods.end()){

          return methods[name];
      }
      if (superclass != nullptr) {
          return superclass->findMethod(name);
        }
      return nullptr;
  }

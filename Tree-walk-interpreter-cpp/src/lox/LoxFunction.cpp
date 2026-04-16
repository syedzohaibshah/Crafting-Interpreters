#include "LoxFunction.h"

#include "LoxCallable.h"
#include "Stmt.h"
#include "Object.h"
#include "Interpreter.h"
#include <variant>
#include "ReturnVal.h"
#include<vector>
#include "Environment.h"


    int LoxFunction:: arity()  {
   //std::cout << "function " << declaration->name.lexeme << " has "
   ///                   << declaration->params.size() << " params.\n";
        return declaration->params.size();
    }

    std::string LoxFunction:: toString() const{
        return "<fn " + declaration->name.lexeme + ">";
    }

    Object  LoxFunction:: call(std::shared_ptr<Interpreter> interpreter,
                const std::vector<Object>& arguments) {

        auto environment = std::make_shared<Environment>(closure);

        for (size_t i = 0; i < declaration->params.size(); i++) {
            environment->define(
                declaration->params[i].lexeme,
                arguments[i]
            );
        }

        try {
          interpreter->executeBlock(declaration->body, environment);
        } catch (ReturnVal returnValue) {
              if (isInitializer) return closure->getAt(0, "this");
          return returnValue.value;
        }


        if (isInitializer) return closure->getAt(0, "this");
        return std::monostate{};
    }

    std::shared_ptr<LoxFunction> LoxFunction::  bind(std::shared_ptr<LoxInstance> instance) {
         auto environment = std::make_shared<Environment>(closure);

       environment->define("this", instance); //intersting situation we have to pass Object
       
       
       return std::make_shared<LoxFunction>(declaration, environment,isInitializer);
     }
     

#pragma once
#include "LoxCallable.h"
#include "Stmt.h"
#include "Object.h"
#include "Interpreter.h"
#include <variant>
#include "ReturnVal.h"
#include<vector>

class LoxFunction : public LoxCallable {

    const Function* declaration;
    std::shared_ptr<Environment> closure;
     bool isInitializer;

public:
    LoxFunction(const Function* declaration,std::shared_ptr<Environment> closure,bool isInitializer)
        : declaration(declaration),closure(closure),isInitializer(isInitializer) {}

    int arity() override {
   //std::cout << "function " << declaration->name.lexeme << " has "
   ///                   << declaration->params.size() << " params.\n";
        return declaration->params.size();
    }

    std::string toString() const override {
        return "<fn " + declaration->name.lexeme + ">";
    }

    Object call(Interpreter* interpreter,
                const std::vector<Object>& arguments) override {

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

    std::shared_ptr<LoxFunction>  bind(std::shared_ptr<LoxInstance> instance) {
         auto environment = std::make_shared<Environment>(closure);

       environment->define("this", instance); //intersting situation we have to pass Object
       
       
       return std::make_shared<LoxFunction>(declaration, environment,isInitializer);
     }
     
     
};

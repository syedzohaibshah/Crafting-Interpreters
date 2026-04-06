#pragma once
#include "LoxCallable.h"
#include "Stmt.h"
#include "Object.h"
#include "Interpreter.h"
#include <variant>
#include "ReturnVal.h"

class LoxFunction : public LoxCallable {

    const Function* declaration;
    std::shared_ptr<Environment> closure; 
public:

    LoxFunction(const Function* declaration,std::shared_ptr<Environment> closure)
        : declaration(declaration),closure(closure) {}

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
          return returnValue.value;
        }



        return std::monostate{};
    }
};

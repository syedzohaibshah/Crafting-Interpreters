#pragma once

#include "LoxCallable.h"
#include "Expr.h"
class LoxCallable;
class Function;
class Environment;

class LoxFunction : public LoxCallable {

    const Function*  declaration;
    std::shared_ptr<Environment> closure;
     bool isInitializer;

public:
    LoxFunction(const Function* declaration, std::shared_ptr<Environment> closure,bool isInitializer)
        : declaration(declaration),closure(closure),isInitializer(isInitializer) {}

    int arity() override ;

    std::string toString() const override ;

    Object call(std::shared_ptr<Interpreter> interpreter,
                const std::vector<Object>& arguments) override ;

    std::shared_ptr<LoxFunction>  bind(std::shared_ptr<LoxInstance> instance) ;
     
     
};

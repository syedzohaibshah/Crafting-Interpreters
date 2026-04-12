#pragma once
#include "Object.h"

class Interpreter;

#include <vector>
class LoxCallable {
public:
    virtual int arity() = 0;

    virtual Object call(Interpreter * interpreter,
                        const std::vector<Object>& arguments) = 0;

    virtual std::string toString() const {
         return "<fn>";
     }

    virtual ~LoxCallable() = default;
};

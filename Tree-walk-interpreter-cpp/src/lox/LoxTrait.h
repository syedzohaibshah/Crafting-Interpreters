#pragma once
#include "LoxFunction.h"
class LoxFunction;

class LoxTrait {
public:

    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;

    LoxTrait(const std::unordered_map<std::string, std::shared_ptr<LoxFunction>>& methods)
        : methods(methods) {}
        
        
};
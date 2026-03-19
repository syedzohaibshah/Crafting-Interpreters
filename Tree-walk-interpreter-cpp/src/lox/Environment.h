#pragma once
#include<iostream>
#include<unordered_map>

#include "Token.h"
#include "RuntimeError.h"

class Environment{
    std::unordered_map<std::string,Object> values;
public:
    Environment(){}

    void define(std::string name,Object value);
    void assign(const Token name, const Object &value);
    Object get(const Token &name);




};

#pragma once
#include<iostream>
#include<unordered_map>
#include "Object.h"
#include "RuntimeError.h"



class Environment{
    std::unordered_map<std::string,Object> values;
    std::shared_ptr<Environment> enclosing;
public:
    Environment(){
        enclosing=nullptr;
    }
 Environment(const std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}
 
    void define(std::string name,Object value);
    void assign(const Token name, const Object & value);
    Object get(const Token &name);
    Object getAt(int distance, std::string name);
    std::shared_ptr<Environment> ancestor(int distance) ;
    void assignAt(int distance, Token name, Object value);



};

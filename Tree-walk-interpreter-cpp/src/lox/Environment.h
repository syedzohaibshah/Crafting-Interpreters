#pragma once



class Environment{
    unordered_map<std::string,Object> values;

    Environment(){}

    void define(std::string name,Object value);
    Object get(const Token &name);




}

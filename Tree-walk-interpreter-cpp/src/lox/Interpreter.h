#pragma once
#include "Expr.h"
#include<any>


class Interpreter: public ExprVisitor{

    std::any visitLiteralExpr(const Literal& expr ){

        return expr.value;
    }


    std::any visitGroupingExpr(const Grouping &expr){


        return evaluate(expr.expression);
    }


    std ::any evaluate(const Expr & expr){

        return expr.accept(expr);
    }


    std::any visitUnaryExpr(const Unary &expr){

        std::any right=evaluate(expr.right);

        switch(expr.op.type){
            case BANG:
            return !is_Truthy(right);
            case MINUS:
              return -std::any_cast<double>(right);

        }


        return nullptr;
    }


    bool is_Truthy( std::any object){

        if(object==nullptr)  return false;
        if(object.type() == typeid(bool)) return std::any_cast<bool>(object);

        return true;
    }

    std::any visitBinaryExpr(const Binary & expr){

        std::any left=evaluate(expr.left);
        std::any right=evaluate(expr.right);

        switch(expr.op.type){


            case MINUS:
              return std::any_cast<double>(left) - std::any_cast<double>(right);
            case SLASH:
                return std::any_cast<double>(left) / std::any_cast<double>(right);
            case STAR:
                return std::any_cast<double>(left) * std::any_cast<double>(right);
            case PLUS:
               if(left.type() == typeid(double) && right.type()==typeid(double)){
                   return std::any_cast<double>(left) + std::any_cast<double>(right);
               }
               if((left.type() == typeid(string)) && (right.type()==typeid(string))){
                   return std::any_cast<string>(left) + std::any_cast<string>(right);
               }
               break;

        }
        return nullptr;
    }

};

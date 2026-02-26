#pragma once
#include "Expr.h"
#include<any>
#include<variant>

class Interpreter: public ExprVisitor{

    VisitorReturn visitLiteralExpr(const Literal& expr ) override{

        return expr.value;
    }


    VisitorReturn visitGroupingExpr(const Grouping &expr) override{


        return evaluate(expr.expression);
    }


    Object evaluate(const Expr & expr) {

        return std::get<Object>(expr.accept(*this));
    }


   VisitorReturn visitUnaryExpr(const Unary &expr){


        std::Object right=evaluate(*expr.right);

        switch(expr.op.type){
            case BANG:
            return !isTruthy(right);
            case MINUS:
              return -std::get<double>(right);

        }


        return nullptr;
    }
/*in LOX
false → false
nil -> false
everything else → true
*/
    bool isTruthy(const Object & object){

        if(std::holds_alternative<std::monostate>(object))  return false;
        if(std::holds_alternative<bool>(object)) return std::get<bool>(object);

        return true;
    }
    //.....
    bool isEqual(const Object& a, const Object& b ){

        if(a==nullptr && b ==nullptr) return true;
        if(a==nullptr) return false;

        return a==b;

    }


   VisitorReturn visitBinaryExpr(const Binary & expr) override{

        Object left=evaluate(*expr.left);
        Object right=evaluate(*expr.right);

        switch(expr.op.type){


            case MINUS:
              return std::get<double>(left) - std::get<double>(right);
            case SLASH:
                return  std::get<double>(left) / std::get<double>(right);
            case STAR:
                return std::get<double>(left) * std::get<double>(right);

            // + for adding doubles and + for string concatination
            case PLUS:
               if(std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                   return std::get<double>(left) + std::get<double>(right);
               }
               if(std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)){
                   return std::get<std::string>(left) + std::get<std::string>(right);
               }
               break;
            case GREATER:
                return std::get<double>(left)> std::get<double>(right);
            case GREATER_EQUAL:
                return std::get<double>(left)>= std::get<double>(right);
            case LESS:
                return std::get<double>(left) < std:get<double>(right);
            case LESS_EQUAL:
                return std::get<double>(left) < std::get<double>(right);

            //equality operators support operands of any type, even mixed ones.
            case BANG_EQUAL:
                return !isEqual(left,right);
            case EQUAL_EQUAL:
                return isEqual(left,right);


        }
        return nullptr;
    }

    VisitorReturn visitConditionalExpr(const Conditional &expr) override{


        Object condition=evaluate(*expr.condition);

        if(isTruthy(condition)){

            return evaluate(*expr.thenBranch);

        }else{

            return evaluate(*expr.elseBranch);
        }


    }

};

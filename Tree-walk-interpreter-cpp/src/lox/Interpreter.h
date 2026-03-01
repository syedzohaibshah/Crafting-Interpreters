#pragma once
#include "Expr.h"
#include<any>
#include<variant>

class Interpreter: public ExprVisitor{

    public:
    VisitorReturn visitLiteralExpr(const Literal& expr ) override{

        return expr.value;
    }


    VisitorReturn visitGroupingExpr(const Grouping &expr) override{


        return evaluate(expr.expression);
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
              check_numbered_operand(expr.op,left,right);
              return std::get<double>(left) - std::get<double>(right);
            case SLASH:
                check_numbered_operand(expr.op,left,right);
                return  std::get<double>(left) / std::get<double>(right);
            case STAR:
                check_numbered_operand(expr.op,left,right);
                return std::get<double>(left) * std::get<double>(right);

            // + for adding doubles and + for string concatination
            case PLUS:
               if(std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                   return std::get<double>(left) + std::get<double>(right);
               }
               if(std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)){
                   return std::get<std::string>(left) + std::get<std::string>(right);
               }
               
               throw RuntimeError(expr.op,"Operator must be two numbers or two strings");
               break;
            case GREATER:
                check_numbered_operand(expr.op,left,right);
                return std::get<double>(left)> std::get<double>(right);
            case GREATER_EQUAL:
                check_numbered_operand(expr.op,left,right);
                return std::get<double>(left)>= std::get<double>(right);
            case LESS:
                check_numbered_operand(expr.op,left,right);
                return std::get<double>(left) < std:get<double>(right);
            case LESS_EQUAL:
                check_numbered_operand(expr.op,left,right);
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

    
    void interpret(const Expr & expr){
        
        try{
            Object value=evaluate(expr);
            std::cout<<stringify(value);
        }catch(const Runtime &error){
        
        Lox::runtimeError(error);
        
    }
        
       private:
      
       //check the operand type first before evaluating
       void check_numbered_operand(const Token & op,const Object & left, const Object & right){
           
           if(std::holds_alternative<double>(right) && std::holds_alternative<double>(left) ) return;
           
           throw RuntimeError(op,"operator must be numbers");
           
       }
        
       Object evaluate(const Expr & expr) {
   
           return std::get<Object>(expr.accept(*this));
       }
    
       //
       std::string stringify(const Object & object){
           
           if (object==nullptr)  return "nil";
           
           if(std::holds_alternative<double>(object)){
               
               std::string text=to_string(object);
               if(text.ends_with(".0")){
                   
                   text=text.substr(0,text.size()-2);
                   
               }
               return text;
           }
           
           return to_string(object);
       }
       
       
       
    
    

};

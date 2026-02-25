#pragma once
#include "Expr.h"



class Interpreter: public ExprVisitor{
    
    std::any visitLiteralExpr(const Literal& expr ){
        
        return expr.value;
    }
    
    
};
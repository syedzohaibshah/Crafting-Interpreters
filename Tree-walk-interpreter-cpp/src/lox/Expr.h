#pragma once
#include "Object.h"
#include <complex>
#include <memory>
#include <utility>
#include<variant>
#include "Token.h"


// Forward declarations
class Binary;
class Grouping;
class Literal;
class Unary;
class Conditional;
class Variable;
class Assign;
class Logical;
class Call;
class FunctionExpr;
class Get;
class Set;
class This;

using VisitorReturn = std::variant<std::string, Object, std::nullptr_t>;

// Expr Visitor interface
class ExprVisitor {
public:
    virtual VisitorReturn visitBinaryExpr(const Binary& expr) = 0;
    virtual VisitorReturn visitGroupingExpr(const Grouping& expr) = 0;
    virtual VisitorReturn visitLiteralExpr(const Literal& expr) = 0;
    virtual VisitorReturn visitUnaryExpr(const Unary& expr) = 0;
    virtual VisitorReturn visitConditionalExpr(const Conditional &expr )=0;
    virtual VisitorReturn visitVariableExpr(const Variable &expr)=0;
    virtual VisitorReturn visitAssignExpr(const Assign &expr)=0;
    virtual VisitorReturn visitLogicalExpr(const Logical & expr)=0;
     virtual VisitorReturn visitCallExpr(const Call & expr)=0;
      virtual VisitorReturn visitGetExpr(const Get & expr)=0;
      virtual VisitorReturn visitSetExpr(const Set & expr)=0;
      virtual VisitorReturn visitThisExpr(const This & expr)=0;


    virtual ~ExprVisitor() = default;
};

// Base class
class Expr {
public:
    virtual VisitorReturn accept(ExprVisitor& visitor) const = 0;
    virtual ~Expr() = default;
};

class Binary : public Expr {
public:
    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    VisitorReturn accept(ExprVisitor& visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }
};

class Grouping : public Expr {
public:
    const std::unique_ptr<Expr> expression;

    explicit Grouping(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    VisitorReturn accept(ExprVisitor& visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }
};

class Literal : public Expr {
public:
    const Object value;

    explicit Literal(Object value) : value(std::move(value)) {}

    VisitorReturn accept(ExprVisitor& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }
};

class Unary : public Expr {
public:
    const Token op;
    const std::unique_ptr<Expr> right;

    Unary(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    VisitorReturn accept(ExprVisitor& visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }
};
//ternary operator a?b:c
class Conditional : public Expr {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> thenBranch;
    std::unique_ptr<Expr> elseBranch;

    Conditional(std::unique_ptr<Expr> condition,
                std::unique_ptr<Expr> thenBranch,
                std::unique_ptr<Expr> elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

          VisitorReturn accept(ExprVisitor& visitor) const override {
              return visitor.visitConditionalExpr(*this);
          }
};


class Variable: public Expr{
    public:
    const Token name;

    Variable(Token name):name(name){}

    VisitorReturn accept(ExprVisitor& visitor) const override {
        return visitor.visitVariableExpr(*this);
    }
};

class Assign : public Expr {
public:
   const Token name;
    std::unique_ptr<Expr> value;

   Assign(const Token name,  std::unique_ptr<Expr> value)
   :name(name),value(std::move(value)){}


 VisitorReturn accept(ExprVisitor& visitor) const override {
     return visitor.visitAssignExpr(*this);
   }


 };


 class Logical: public Expr{
     public:
     std::unique_ptr<Expr> left;
     Token optr;
     std::unique_ptr<Expr> right;

     Logical(std::unique_ptr<Expr> left,Token optr,std::unique_ptr<Expr> right):
     left(std::move(left)),optr(optr),right(std::move(right)){}

     VisitorReturn accept(ExprVisitor& visitor) const override {
         return visitor.visitLogicalExpr(*this);
       }


 };


 class Call :public Expr{
     public:
     std::unique_ptr<Expr> callee;
     Token paren;
     std::vector<std::unique_ptr<Expr>> arguments;

     Call(std::unique_ptr<Expr> callee,Token paren,   std::vector<std::unique_ptr<Expr>> arguments)
     :callee(std::move(callee)),paren(paren),arguments(std::move(arguments)){}


     VisitorReturn accept(ExprVisitor& visitor) const override {
         return visitor.visitCallExpr(*this);
       }
 };


 class Get :public Expr{
     public:
     std::unique_ptr<Expr> object;
     Token name;


     Get(std::unique_ptr<Expr> object,Token name)
     :object(std::move(object)),name(name){}


     VisitorReturn accept(ExprVisitor& visitor) const override {
         return visitor.visitGetExpr(*this);
       }
 };


 class Set :public Expr{
     public:
     std::unique_ptr<Expr> object;
     Token name;
     std::unique_ptr<Expr> value;


     Set(std::unique_ptr<Expr> object,Token name,std::unique_ptr<Expr> value)
     :object(std::move(object)),name(name),value(std::move(value)){}


     VisitorReturn accept(ExprVisitor& visitor) const override {
         return visitor.visitSetExpr(*this);
       }

 };



 class This :public Expr{
     public:

     Token keyword;



     This(Token keyword) :keyword(keyword){}


     VisitorReturn accept(ExprVisitor& visitor) const override {
         return visitor.visitThisExpr(*this);
       }

 };

#pragma once

#include "Token.h"
#include <vector>
#include "Expr.h"
class Expression;
class Print;
class Var;
class Block ;
class If;
class While;
class Break;
class Function;
class Return;
class Class;

class StmtVisitor{
    public:
   virtual  void visitPrintStmt(const Print& stmt)=0;
   virtual  void visitExpressionStmt(const Expression &stmt)=0;
   virtual  void visitVarStmt(const Var & stmt)=0;
   virtual  void visitBlockStmt(const Block& stmt)=0;
   virtual void  visitIfStmt(const If& stmt)=0;
   virtual void visitWhileStmt(const While& stmt)=0;
   virtual void visitBreakStmt(const Break& stmt)=0;
   virtual void visitFunctionStmt(const Function & stmt)=0;
   virtual void visitReturnStmt(const Return & stmt)=0;
   virtual void visitClassStmt(const Class & stmt)=0;


   virtual ~StmtVisitor()=default;
};


//base class
class Stmt{
public:
virtual void accept(StmtVisitor & visitor) const=0;
virtual ~Stmt()=default;

};

//Stmt visitor interface



class Expression :public Stmt{
    public:
     const std::unique_ptr<Expr> expression;

     Expression(std::unique_ptr<Expr> expr ): expression(std::move(expr)) {}


     void accept(StmtVisitor &visitor)const override{

       visitor.visitExpressionStmt(*this);

     }

};


class Print :public Stmt{

    public:
     const std::unique_ptr<Expr>  expression;



     Print(std::unique_ptr<Expr> expr ):expression(std::move(expr)) {}


     void accept(StmtVisitor &visitor)const override{

       visitor.visitPrintStmt(*this);

     }
};


class Var :public Stmt{
public:
    const std::unique_ptr<Expr> initializer;
    const Token name;

    Var(Token name, std::unique_ptr<Expr> initializer)
        :  initializer(std::move(initializer)),name(name) {}

    void accept(StmtVisitor &visitor)const override{

      visitor.visitVarStmt(*this);

    }



};


class Block :public  Stmt {
    public:
    std::vector<std::unique_ptr<Stmt>> statements;

  Block( std::vector<std::unique_ptr<Stmt>> &&statements): statements(std::move(statements)) {}


void accept(StmtVisitor & visitor) const override{
    return visitor.visitBlockStmt(*this);
  }


};


class If :public  Stmt {
    public:
    std::unique_ptr<Expr> condition;
     std::unique_ptr<Stmt> thenBranch;
     std::unique_ptr<Stmt> elseBranch;

  If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch,  std::unique_ptr<Stmt> elseBranch)
 :condition(std::move(condition)),thenBranch(std::move(thenBranch)),elseBranch(std::move(elseBranch)){}

void  accept(StmtVisitor & visitor) const override {
    return visitor.visitIfStmt(*this);
  }


};

class While: public Stmt{

    public:
    std::unique_ptr<Expr> condition;
     std::unique_ptr<Stmt> body;

    While( std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
    :condition(std::move(condition)),body(std::move(body)){}

    void  accept(StmtVisitor & visitor) const override {
        return visitor.visitWhileStmt(*this);
      }

};


class Break :public Stmt{

    public:


    Break(){}



    void  accept(StmtVisitor & visitor) const override {
        return visitor.visitBreakStmt(*this);
      }


};


class Function :public Stmt{
    public :

    Token name;
    std::vector<Token> params;
    std::vector< std::unique_ptr<Stmt>>body;

    Function(Token name,
             std::vector<Token> params,
             std::vector<std::unique_ptr<Stmt>> body)
      : name(std::move(name)),
        params(std::move(params)),
        body(std::move(body)) {}



    void  accept(StmtVisitor & visitor) const override {
        return visitor.visitFunctionStmt(*this);
      }




};


class Return :public Stmt{
public:
     Token keyword;
    std::unique_ptr<Expr> value;
    Return (Token keyword,std::unique_ptr<Expr> value):keyword(keyword),value(std::move(value)){}

    void  accept(StmtVisitor & visitor) const override {
        return visitor.visitReturnStmt(*this);
      }


};


class Class :public Stmt{
public:
     Token name;
     std::vector< std::unique_ptr<Function>>methods;
     std::vector<std::unique_ptr<Function>> staticMethods;
     
    Class(Token name,std::vector<std::unique_ptr<Function>>methods,
        std::vector<std::unique_ptr<Function>> staticMethods)
    :name(name),methods(std::move(methods)),staticMethods(std::move(staticMethods)){}

    void  accept(StmtVisitor & visitor) const override {
        return visitor.visitClassStmt(*this);
      }


};

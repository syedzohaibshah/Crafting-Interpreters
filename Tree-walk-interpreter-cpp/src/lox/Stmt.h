#pragma once


class Expression;
class Print;


class StmtVisitor{
    public:
   virtual  void visitPrintStmt(const Print& stmt)=0;
   virtual  void visitExpressionStmt(const Expression &stmt)=0;

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
    
    const std::unique_ptr<Expr> initializer;
    const Token operator;
    
    
    
    
    
};
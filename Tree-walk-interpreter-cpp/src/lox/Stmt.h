#pragma once


class Expression;
class Print;
class Var;
class Block ;

class StmtVisitor{
    public:
   virtual  void visitPrintStmt(const Print& stmt)=0;
   virtual  void visitExpressionStmt(const Expression &stmt)=0;
   virtual  void visitVarStmt(const Var & stmt)=0;
   virtual  void visitBlockStmt(const Block& stmt)=0;

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

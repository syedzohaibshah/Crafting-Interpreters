#pragma once
#include "Interpreter.h"
#include "Expr.h"
#include "Stmt.h"
class Interpreter;




#include <memory>
class Resolver : public ExprVisitor,public StmtVisitor{


    std::vector<std::unordered_map<std::string, bool>> scopes;
    Interpreter* interpreter;
   

      Resolver(Interpreter interpreter):interpreter(std::move(interpreter)){}

    //helpers:
    //
    void resolve(std::vector<Stmt> statements);
    void resolve(const Stmt &  stmt);
    void resolve(const Expr &expr);
    void  beginScope();
     void  endScope() ;
     void declare(Token name);
     void  define(Token name);
     
   void resolveLocal(std::unique_ptr<Expr>expr, const Token& name);
   void   resolveFunction(const Function & function);
 public:






  void visitBlockStmt(const Block & stmt) override;
  void visitVarStmt(const Var & stmt) override;
  void visitFunctionStmt(const Function & stmt)override;
  void visitExpressionStmt(const Expression &stmt)override;
  void  visitIfStmt(const If &stmt)override;
  void visitPrintStmt(const Print &stmt)override;
  void visitReturnStmt(const Return &stmt)override;
  void  visitWhileStmt(const While & stmt) override;

void visitVariableExpr(const Variable &expr) override;
void visitAssignExpr(const Assign & expr)override;
void visitBinaryExpr(const Binary& expr) override;
void  visitCallExpr(const Call &expr) override;
void  visitGroupingExpr(const Grouping &expr)override;
void  visitLiteralExpr(const Literal& expr)override;
void visitLogicalExpr(const Logical &expr)override;
void  visitUnaryExpr(const Unary &expr)override;




};

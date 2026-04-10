#pragma once
#include "Interpreter.h"
#include "Expr.h"
#include "Stmt.h"
#include "FunctionType.h"
#include <memory>



class Resolver : public ExprVisitor,public StmtVisitor{


    std::vector<std::unordered_map<std::string, bool>> scopes;
    FunctionType currentFunction = FunctionType::NONE;
    Interpreter* interpreter;


    //helpers:
    //

    void  beginScope();
     void  endScope() ;
     void declare(Token name);
     void  define(Token name);

   void resolveLocal(const Expr & expr, const Token& name);
   void   resolveFunction(const Function & function,FunctionType func);
 public:
 void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);
 void resolve(const Stmt &stmt);
 void resolve(const Expr &expr);


 Resolver(Interpreter* interpreter):interpreter(interpreter){}




  void visitBlockStmt(const Block & stmt) override;
  void visitVarStmt(const Var & stmt) override;
  void visitFunctionStmt(const Function & stmt)override;
  void visitExpressionStmt(const Expression &stmt)override;
  void  visitIfStmt(const If &stmt)override;
  void visitPrintStmt(const Print &stmt)override;
  void visitReturnStmt(const Return &stmt)override;
  void  visitWhileStmt(const While & stmt) override;
 void visitBreakStmt(const Break& stmt)override;
 void visitClassStmt(const Class &stmt) override;
 
 
VisitorReturn visitVariableExpr(const Variable &expr) override;
VisitorReturn visitAssignExpr(const Assign & expr)override;
VisitorReturn visitBinaryExpr(const Binary& expr) override;
VisitorReturn  visitCallExpr(const Call &expr) override;
VisitorReturn  visitGroupingExpr(const Grouping &expr)override;
VisitorReturn  visitLiteralExpr(const Literal& expr)override;
VisitorReturn visitLogicalExpr(const Logical &expr)override;
VisitorReturn  visitUnaryExpr(const Unary &expr)override;
VisitorReturn visitConditionalExpr(const Conditional &expr )override;




};

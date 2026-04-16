#pragma once
#include "Expr.h"
#include "LoxClass.h"
#include"Stmt.h"
#include <future>          //observations:: stsmt return voi and expr return Object
#include <memory>
#include <unordered_map>
#include <variant>
#include <string>
#include "Environment.h"
#include "LoxCallable.h"
#include "ClockFunction.h"
#include "Object.h"

class Token;
class RuntimeError;
class LoxClass;

class BreakException {};

class Interpreter : public ExprVisitor,public StmtVisitor,public std::enable_shared_from_this<Interpreter> {  //...
private:


    void check_numbered_operand(const Token& op, const Object& left, const Object& right);
    Object evaluate(const Expr& expr);
    std::string stringify(const Object& object);
    bool isTruthy(const Object& object);
    bool isEqual(const Object& a, const Object& b);
    void execute(const Stmt &stmt);
    std::shared_ptr<LoxClass> asClass(const Object& obj) ;
        std::shared_ptr<LoxInstance> asInstance(const Object& obj);

VisitorReturn  lookUpVariable(Token name, const Expr &expr);

public:

std::shared_ptr<Environment> globals;

 std::shared_ptr<Environment>  environment;
 std::unordered_map<const Expr*, int> locals;

    VisitorReturn visitLiteralExpr(const Literal& expr) override;
    VisitorReturn  visitLogicalExpr(const Logical& expr)override;
    VisitorReturn visitGroupingExpr(const Grouping& expr) override;
    VisitorReturn visitUnaryExpr(const Unary& expr) override;
    VisitorReturn visitBinaryExpr(const Binary& expr) override;
    VisitorReturn visitConditionalExpr(const Conditional& expr) override;
    VisitorReturn visitVariableExpr(const Variable &expr) override;
    VisitorReturn visitAssignExpr(const Assign &expr) override;
    VisitorReturn visitCallExpr(const Call&expr) override;
    VisitorReturn  visitGetExpr(const Get& expr) override;
VisitorReturn  visitSetExpr(const Set& expr) override;
VisitorReturn visitThisExpr(const This &expr) override;
VisitorReturn visitSuperExpr(const Super & expr) override;


    void visitExpressionStmt(const Expression & stmt)override;
    void visitPrintStmt(const Print & stmt) override;
    void visitVarStmt(const Var &stmt) override;
    void visitBlockStmt(const Block & stmt) override;
    void visitIfStmt(const If & stmt) override;
    void visitWhileStmt(const While & stmt) override;
    void visitBreakStmt(const Break& stmt) override;
    void visitFunctionStmt(const Function& stmt)override;
     void visitReturnStmt(const Return & stmt) override;
     void visitClassStmt(const Class & stmt) override;


    void executeBlock( const std::vector<std::unique_ptr<Stmt>>&  statements,
                      std::shared_ptr<Environment> environment) ;


    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);

    void resolve(const Expr & expr, int depth);

    // Interpreter() : environment(std::make_shared<Environment>()) {}
Interpreter() {
    globals = std::make_shared<Environment>();
           environment = globals;

           globals->define("clock",
                  std::make_shared<ClockFunction>());

}


};

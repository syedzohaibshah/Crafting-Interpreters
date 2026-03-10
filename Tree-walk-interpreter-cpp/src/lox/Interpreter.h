#pragma once
#include "Expr.h"
#include"Stmt.h"
#include <variant>
#include <string>

class Token;
class RuntimeError;

class Interpreter : public ExprVisitor,public StmtVisitor {  //...
private:
Environment environment;

    void check_numbered_operand(const Token& op, const Object& left, const Object& right);
    Object evaluate(const Expr& expr);
    std::string stringify(const Object& object);
    bool isTruthy(const Object& object);
    bool isEqual(const Object& a, const Object& b);
    void execute(const Stmt &stmt);

public:
    VisitorReturn visitLiteralExpr(const Literal& expr) override;
    VisitorReturn visitGroupingExpr(const Grouping& expr) override;
    VisitorReturn visitUnaryExpr(const Unary& expr) override;
    VisitorReturn visitBinaryExpr(const Binary& expr) override;
    VisitorReturn visitConditionalExpr(const Conditional& expr) override;

    void visitExpressionStmt(const Expression & stmt)override;
    void visitPrintStmt(const Print & stmt) override;
void visitVarStm(const Var &stmt) override;
    void interpret(std::vector<std::unique_ptr<Stmt>> &statements);
};

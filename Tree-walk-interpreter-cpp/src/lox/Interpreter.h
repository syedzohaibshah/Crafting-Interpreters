#pragma once
#include "Expr.h"
#include <variant>
#include <string>

class Token;
class RuntimeError;

class Interpreter : public ExprVisitor {
private:
    void check_numbered_operand(const Token& op, const Object& left, const Object& right);
    Object evaluate(const Expr& expr);
    std::string stringify(const Object& object);
    bool isTruthy(const Object& object);
    bool isEqual(const Object& a, const Object& b);

public:
    VisitorReturn visitLiteralExpr(const Literal& expr) override;
    VisitorReturn visitGroupingExpr(const Grouping& expr) override;
    VisitorReturn visitUnaryExpr(const Unary& expr) override;
    VisitorReturn visitBinaryExpr(const Binary& expr) override;
    VisitorReturn visitConditionalExpr(const Conditional& expr) override;
    void interpret(const Expr& expr);
};

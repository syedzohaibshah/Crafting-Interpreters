#pragma once
#include "Token.h"
#include <any>
#include <memory>
#include <utility>

// Forward declarations
class Binary;
class Grouping;
class Literal;
class Unary;
class Conditional;

// Visitor interface
class ExprVisitor {
public:
    virtual std::any visitBinaryExpr(const Binary& expr) = 0;
    virtual std::any visitGroupingExpr(const Grouping& expr) = 0;
    virtual std::any visitLiteralExpr(const Literal& expr) = 0;
    virtual std::any visitUnaryExpr(const Unary& expr) = 0;
    virtual std::any visitConditionalExpr(const Conditional &expr )=0;
    virtual ~ExprVisitor() = default;
};

// Base class
class Expr {
public:
    virtual std::any accept(ExprVisitor& visitor) const = 0;
    virtual ~Expr() = default;
};

class Binary : public Expr {
public:
    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }
};

class Grouping : public Expr {
public:
    const std::unique_ptr<Expr> expression;

    explicit Grouping(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }
};

class Literal : public Expr {
public:
    const Object value;

    explicit Literal(Object value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }
};

class Unary : public Expr {
public:
    const Token op;
    const std::unique_ptr<Expr> right;

    Unary(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
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

          std::any accept(ExprVisitor& visitor) const override {
              return visitor.visitConditionalExpr(*this);
          }
};

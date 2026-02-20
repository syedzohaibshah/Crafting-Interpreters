#pragma once

#include <any>
#include <string>
#include <sstream>
#include <initializer_list>
#include "Expr.h"

class AstPrinter : public ExprVisitor {
public:
    std::string print(const Expr& expr) {
        return std::any_cast<std::string>(expr.accept(*this));
    }

    std::any visitBinaryExpr(const Binary& expr) override {
        return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
    }

    std::any visitGroupingExpr(const Grouping& expr) override {
        return parenthesize("group", {expr.expression.get()});
    }

    std::any visitLiteralExpr(const Literal& expr) override {
        if (std::holds_alternative<std::monostate>(expr.value))
            return std::string("nil");
        if (std::holds_alternative<double>(expr.value))
            return std::to_string(std::get<double>(expr.value));
        if (std::holds_alternative<std::string>(expr.value))
            return std::get<std::string>(expr.value);
        if (std::holds_alternative<bool>(expr.value))
            return std::string(std::get<bool>(expr.value) ? "true" : "false");
        return std::string("");
    }

    std::any visitUnaryExpr(const Unary& expr) override {
        return parenthesize(expr.op.lexeme, {expr.right.get()});
    }

private:
    std::string parenthesize(const std::string& name,
                             std::initializer_list<const Expr*> exprs) {
        std::ostringstream ss;
        ss << "(" << name;
        for (const Expr* expr : exprs) {
            ss << " " << std::any_cast<std::string>(expr->accept(*this));
        }
        ss << ")";
        return ss.str();
    }
};

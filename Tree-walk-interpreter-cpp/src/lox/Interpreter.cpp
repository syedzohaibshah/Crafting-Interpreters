#include "Interpreter.h"
#include "RuntimeError.h"
#include "Token.h"
#include "Lox.h"
#include <iostream>
#include <variant>

void Interpreter::check_numbered_operand(const Token& op, const Object& left, const Object& right) {
    if (std::holds_alternative<double>(right) && std::holds_alternative<double>(left)) return;
    throw RuntimeError(op, "operator must be numbers");
}
//important
Object Interpreter::evaluate(const Expr& expr) {
    return std::get<Object>(expr.accept(*this));
}



std::string Interpreter::stringify(const Object& object) {
    if (std::holds_alternative<std::monostate>(object)) return "nil";

    if (std::holds_alternative<double>(object)) {
        std::string text = std::to_string(std::get<double>(object));
        if (text.ends_with(".000000")) {
            text = text.substr(0, text.size() - 7);
        }
        return text;
    }

    if (std::holds_alternative<std::string>(object)) {
        return std::get<std::string>(object);
    }

    if (std::holds_alternative<bool>(object)) {
        return std::get<bool>(object) ? "true" : "false";
    }

    throw std::runtime_error("Unexpected object type");
}




bool Interpreter::isTruthy(const Object& object) {
    if (std::holds_alternative<std::monostate>(object)) return false;
    if (std::holds_alternative<bool>(object)) return std::get<bool>(object);
    return true;
}

bool Interpreter::isEqual(const Object& a, const Object& b) {
    if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) return true;
    if (std::holds_alternative<std::monostate>(a)) return false;
    return a == b;
}

VisitorReturn Interpreter::visitLiteralExpr(const Literal& expr) {
    return expr.value;
}

VisitorReturn Interpreter:: visitLogicalExpr(const Logical& expr) {
   Object left = evaluate(*expr.left);

   if (expr.optr.type == OR) {
     if (isTruthy(left)) return left;
   } else {
     if (!isTruthy(left)) return left;
   }

   return evaluate(*expr.right);
 }

VisitorReturn Interpreter::visitGroupingExpr(const Grouping& expr) {
    return evaluate(*expr.expression);
}

VisitorReturn Interpreter::visitUnaryExpr(const Unary& expr) {
    Object right = evaluate(*expr.right);

    switch (expr.op.type) {
        case BANG:
            return !isTruthy(right);
        case MINUS:
            if (!std::holds_alternative<double>(right)) {
                throw RuntimeError(expr.op, "Operand must be a number.");
            }
            return Object{-std::get<double>(right)};
        default: break;
    }

    return std::monostate{};
}


VisitorReturn Interpreter:: visitVariableExpr(const Variable &expr) {
  return environment->get(expr.name);
}

VisitorReturn Interpreter:: visitAssignExpr(const Assign & expr) {
         Object value = evaluate(*expr.value);
         environment->assign(expr.name, value);
         return value;
       }

VisitorReturn Interpreter::visitBinaryExpr(const Binary& expr) {
    Object left = evaluate(*expr.left);
    Object right = evaluate(*expr.right);

    switch (expr.op.type) {
        case MINUS:
            check_numbered_operand(expr.op, left, right);
            return Object{std::get<double>(left) - std::get<double>(right)};
        case SLASH:
            check_numbered_operand(expr.op, left, right);
            if(std::get<double>(right)==0){
                 throw RuntimeError(expr.op, "divsion by ZERO");
            }
            return Object{std::get<double>(left) / std::get<double>(right)};
        case STAR:
            check_numbered_operand(expr.op, left, right);
            return Object{std::get<double>(left) * std::get<double>(right)};
        case PLUS:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return Object{std::get<double>(left) + std::get<double>(right)};
            }
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                return Object{std::get<std::string>(left) + std::get<std::string>(right)}       ;
            }
            if (std::holds_alternative<double>(left) && std::holds_alternative<std::string>(right)) {
                return Object{stringify(left)+ std::get<std::string>(right)};
            }
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<double>(right)) {
                return Object{std::get<std::string>(left) + stringify(right)};
            }
            throw RuntimeError(expr.op, "Operator must be two numbers or two strings");
        case GREATER:
            check_numbered_operand(expr.op, left, right);
            return Object{std::get<double>(left) > std::get<double>(right)};
        case GREATER_EQUAL:
            check_numbered_operand(expr.op, left, right);
            return Object{std::get<double>(left) >= std::get<double>(right)};
        case LESS:
            check_numbered_operand(expr.op, left, right);
            return Object{std::get<double>(left) < std::get<double>(right)};
        case LESS_EQUAL:
            check_numbered_operand(expr.op, left, right);
            return Object{std::get<double>(left) <= std::get<double>(right)};
        case BANG_EQUAL:
            return Object{!isEqual(left, right)};
        case EQUAL_EQUAL:
            return Object{isEqual(left, right)};
        default: break;
    }
    return std::monostate{};
}

VisitorReturn Interpreter::visitConditionalExpr(const Conditional& expr) {
    Object condition = evaluate(*expr.condition);

    if (isTruthy(condition)) {
        return evaluate(*expr.thenBranch);
    } else {
        return evaluate(*expr.elseBranch);
    }
}


void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>> &statements) {
    try {
        for(const auto & stmt:statements){
            execute(*stmt);
        }
    } catch (const RuntimeError& error) {
        Lox::runtimeError(error);
    }
    catch (const std::exception& ex) {
        std::cerr << "Interpreter internal error: " << ex.what() << '\n';
    }
}


void Interpreter:: execute(const Stmt &stmt){
    stmt.accept(*this);
}

void Interpreter:: visitBlockStmt(const Block & stmt) {
     auto blockEnv = std::make_shared<Environment>(environment);
  executeBlock(stmt.statements, blockEnv);

}


void  Interpreter:: executeBlock(const std::vector<std::unique_ptr<Stmt>>&  statements,
                  std::shared_ptr<Environment> environment) {                           //handling scope
  auto previous = this->environment;
  try {
    this->environment = environment;

    for (const auto &statement : statements) {
      execute(*statement);
    }
  } catch(...) {
    this->environment = previous;
    throw;
  }
  this->environment = previous;
}



void Interpreter::visitExpressionStmt(const Expression & stmt){

    evaluate(*stmt.expression);

}

void Interpreter::visitPrintStmt(const Print & stmt){

    Object value=evaluate(*stmt.expression);
    std::cout<<stringify(value)<<'\n';
}

void Interpreter::visitVarStmt(const Var &stmt) {

    Object value = std::monostate{};
        if (stmt.initializer != nullptr) {
          value = evaluate(*stmt.initializer);
        }

        environment->define(stmt.name.lexeme, value);

      }


void Interpreter::visitIfStmt(const If & stmt) {

        if (isTruthy(evaluate(*stmt.condition))) {
          execute(*stmt.thenBranch.get());
        } else if (stmt.elseBranch != nullptr) {
          execute(*stmt.elseBranch.get());
        }

      }


void Interpreter:: visitWhileStmt(const While & stmt) {
    try {
          while (isTruthy(evaluate(*stmt.condition.get()))) {
              try {
                  execute(*stmt.body.get());
              } catch (BreakException&) {
                  break;
              }
          }
      } catch (...) {
          throw; // propagate other exceptions
      }

      }


      void Interpreter::visitBreakStmt(const Break& stmt) {
          throw BreakException();
      }

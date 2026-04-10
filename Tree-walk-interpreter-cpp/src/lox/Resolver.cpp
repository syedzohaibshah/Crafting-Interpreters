#include "Resolver.h"
#include "Stmt.h"
#include <memory>
#include <unordered_map>
#include <variant>
#include "Lox.h"
#include "Interpreter.h"

void Resolver:: visitBlockStmt(const Block &stmt) {
  beginScope();
  resolve(stmt.statements); //issue
  endScope();

}

void  Resolver :: resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {

  for (auto  & statement : statements) {
    resolve(*statement);
  }
}




 void  Resolver ::resolve(const Stmt &  stmt) {
  stmt.accept(*this);
}


void  Resolver ::resolve(const Expr &  expr) {
 expr.accept(*this);
}


void  Resolver :: beginScope() {
  scopes.push_back(std::unordered_map<std::string, bool>());
}


 void Resolver :: endScope() {
  scopes.pop_back();
}



void Resolver :: visitVarStmt(const Var & stmt) {

    declare(stmt.name);
    if (stmt.initializer != nullptr) {
      resolve(*stmt.initializer);
    }
    define(stmt.name);

  }


void Resolver :: declare(Token name) {
    if (scopes.empty()) return;


    auto& scope = scopes.back();
    if (scope.find(name.lexeme)!=scope.end()) {
      Lox::error(name,
          "Already a variable with this name in this scope.");
    }

    scope[name.lexeme]= false;

  }

void Resolver ::  define(Token name) {
    if (scopes.empty()) return;
    scopes.back()[name.lexeme]= true;
  }


  VisitorReturn Resolver::visitVariableExpr(const Variable & expr) {
      // Check if there is a local scope
      if (!scopes.empty()) {
          auto& innermost = scopes.back();
          auto it = innermost.find(expr.name.lexeme);
          if (it != innermost.end() && it->second == false) {
              Lox::error(expr.name,
                         "Can't read local variable in its own initializer.");
          }
      }

      // Resolve variable to a specific scope
      resolveLocal(expr, expr.name);

 return std::monostate{};
  }


  void Resolver::resolveLocal(const Expr& expr, const Token& name) {

      for (int i = scopes.size() - 1; i >= 0; --i) {
          if (scopes[i].find(name.lexeme) != scopes[i].end()) {
              int distance = scopes.size() - 1 - i;
              interpreter->resolve(expr, distance);
              return;
          }
      }
  }


  VisitorReturn Resolver:: visitAssignExpr(const Assign & expr) {
    resolve(*expr.value);
    resolveLocal(expr, expr.name);
 return std::monostate{};
  }

  void Resolver:: visitFunctionStmt(const Function & stmt) {
      declare(stmt.name);
      define(stmt.name);

      resolveFunction(stmt,FunctionType::FUNCTION);

    }

 void Resolver::resolveFunction(const Function & function,FunctionType type) {
     FunctionType enclosingFunction = currentFunction;
         currentFunction = type;
       beginScope();
       for (Token param : function.params) {
         declare(param);
         define(param);
       }
       resolve(function.body);
       endScope();
        currentFunction = enclosingFunction;
     }


void Resolver:: visitExpressionStmt(const Expression &stmt) {
       resolve(*stmt.expression);

     }

void Resolver:: visitIfStmt(const If &stmt) {
       resolve(*stmt.condition);
       resolve(*stmt.thenBranch);
       if (stmt.elseBranch != nullptr) resolve(*stmt.elseBranch);

     }


void Resolver:: visitPrintStmt(const Print &stmt) {
       resolve(*stmt.expression);

     }

void Resolver:: visitReturnStmt(const Return &stmt) {
    if (currentFunction == FunctionType::NONE) {
      Lox::error(stmt.keyword, "Can't return from top-level code.");
    }
       if (stmt.value != nullptr) {
         resolve(*stmt.value);
       }


     }


void Resolver:: visitWhileStmt(const While & stmt) {
       resolve(*stmt.condition);
       resolve(*stmt.body);

     }

    void Resolver:: visitBreakStmt(const Break& stmt){

        //nothing
    }

    
    
 void Resolver:: visitClassStmt(const Class &stmt) {
        declare(stmt.name);
        define(stmt.name);
        
      }
      
     VisitorReturn Resolver:: visitBinaryExpr(const Binary& expr) {
       resolve(*expr.left);
       resolve(*expr.right);
      return std::monostate{};
     }


     VisitorReturn Resolver:: visitCallExpr(const Call &expr) {
       resolve(*expr.callee);

       for (auto & argument : expr.arguments) {
         resolve(*argument);
       }

        return std::monostate{};
     }


     VisitorReturn Resolver:: visitGroupingExpr(const Grouping &expr) {
       resolve(*expr.expression);
       return std::monostate{};
     }


     VisitorReturn Resolver:: visitLiteralExpr(const Literal& expr) {
       return std::monostate{};
     }


     VisitorReturn Resolver:: visitLogicalExpr(const Logical &expr) {
       resolve(*expr.left);
       resolve(*expr.right);
      return std::monostate{};
     }


     VisitorReturn Resolver:: visitUnaryExpr(const Unary &expr) {
       resolve(*expr.right);
        return std::monostate{};
     }

     VisitorReturn Resolver:: visitConditionalExpr(const Conditional &expr ){

         resolve(*expr.condition);
         resolve(*expr.thenBranch);
         resolve(*expr.elseBranch);
         
         return std::monostate{};

     }

     

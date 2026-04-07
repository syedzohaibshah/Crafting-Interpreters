#include "Resolver.h"
#include "Stmt.h"
#include <memory>
#include <unordered_map>
#include "Lox.h"

void Resolver:: visitBlockStmt(const Block &stmt) {
  beginScope();
  resolve(stmt.statements); //issue
  endScope();

}

void  Resolver :: resolve(std::vector<Stmt> statements) {

  for (auto statement : statements) {
    resolve(statement);
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
      resolve(stmt.initializer);
    }
    define(stmt.name);

  }


void Resolver :: declare(Token name) {
    if (scopes.empty()) return;
    
  
    std::unordered_map<std::string, bool> scope = scopes.back();
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


  void Resolver::visitVariableExpr(const Variable & expr) {
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


  }


  void Resolver::resolveLocal(std::unique_ptr<Expr>expr, const Token& name) {

      for (int i = scopes.size() - 1; i >= 0; --i) {
          if (scopes[i].find(name.lexeme) != scopes[i].end()) {
              int distance = scopes.size() - 1 - i;
              interpreter->resolve(expr, distance);
              return;
          }
      }
  }


  void Resolver:: visitAssignExpr(const Assign & expr) {
    resolve(expr.value);
    resolveLocal(expr, expr.name);

  }

  void Resolver:: visitFunctionStmt(const Function & stmt) {
      declare(stmt.name);
      define(stmt.name);

      resolveFunction(stmt);

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
       resolve(stmt.expression);
   
     }
     
void Resolver:: visitIfStmt(const If &stmt) {
       resolve(stmt.condition);
       resolve(stmt.thenBranch);
       if (stmt.elseBranch != nullptr) resolve(stmt.elseBranch);
      
     }   
     

void Resolver:: visitPrintStmt(const Print &stmt) {
       resolve(stmt.expression);
   
     }     
     
void Resolver:: visitReturnStmt(const Return &stmt) {
    if (currentFunction == FunctionType::NONE) {
      Lox::error(stmt.keyword, "Can't return from top-level code.");
    }
       if (stmt.value != nullptr) {
         resolve(stmt.value);
       }
   
    
     }

     
void Resolver:: visitWhileStmt(const While & stmt) {
       resolve(stmt.condition);
       resolve(stmt.body);
      
     }

     
     
     void Resolver:: visitBinaryExpr(const Binary& expr) {
       resolve(expr.left);
       resolve(expr.right);
       return null;
     }
     
     
     void Resolver:: visitCallExpr(const Call &expr) {
       resolve(expr.callee);
   
       for (Expr argument : expr.arguments) {
         resolve(argument);
       }
   
       return null;
     }

     
     void Resolver:: visitGroupingExpr(const Grouping &expr) {
       resolve(expr.expression);
       return null;
     }
     
     
     void Resolver:: visitLiteralExpr(const Literal& expr) {
       return null;
     }
     
     
     void Resolver:: visitLogicalExpr(const Logical &expr) {
       resolve(expr.left);
       resolve(expr.right);
       return null;
     }
     
     
     void Resolver:: visitUnaryExpr(const Unary &expr) {
       resolve(expr.right);
       return null;
     }
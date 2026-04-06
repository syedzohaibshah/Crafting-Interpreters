
#include "Parser.h"
#include <initializer_list>
#include <memory>
#include <vector>

#include "Lox.h"
#include "Stmt.h"
#include "Token.h"
std::unique_ptr<Expr> Parser::expression(){

     return   assignment();// conditional();
 }

 std::unique_ptr<Expr> Parser::assignment() {
     auto expr =    conditional();

     if (match({EQUAL})) {
       Token equals = previous();
       auto value = assignment();

       if (auto var = dynamic_cast<Variable*>(expr.get())) {
         Token name = var->name;

         expr = std::make_unique<Assign>(
             name,
             std::move(value));

         return expr;
       }

       error(equals, "Invalid assignment target.");
     }

     return expr;
   }



   std::unique_ptr<Expr> Parser::conditional() {
       auto expr = orExpr() ;

       if (match({TokenType::QUESTION})) {
           auto thenBranch = expression();

           consume(TokenType::COLON,
                   "Expect ':' after then branch.");

           auto elseBranch = conditional(); // RIGHT associative

           expr = std::make_unique<Conditional>(
               std::move(expr),
               std::move(thenBranch),
               std::move(elseBranch)
           );
       }

       return expr;
   }


  std::unique_ptr<Expr> Parser:: orExpr() {
      auto expr = andExpr();

      while (match({OR})) {
        Token optr = previous();
        auto right = andExpr();
        expr = std::make_unique<Logical>(std::move(expr), optr, std::move( right));
      }

      return expr;
    }

  std::unique_ptr<Expr> Parser::  andExpr() {
      auto expr = equality();

      while (match({AND})) {
        Token optr= previous();
        auto right =equality();
        expr = std::make_unique<Logical>(std::move(expr), optr, std::move( right));
      }

      return expr;
    }


std::unique_ptr<Expr> Parser::equality(){
    auto expr=comparison();

    while(match({BANG_EQUAL,EQUAL_EQUAL})){
        Token op=previous();
        auto right=comparison();

        expr = std::make_unique<Binary>(
            std::move(expr),
            op,
            std::move(right));

    }
    return expr;

}


// comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;

std::unique_ptr<Expr> Parser::comparison(){


    auto  expr=term();
    while(match({GREATER,GREATER_EQUAL,LESS,LESS_EQUAL})){
        Token op=previous();
        auto right=term();
        expr=std::make_unique<Binary>(
            std::move(expr),
            op,
            std::move(right));

    }

    return expr;
}


std::unique_ptr<Expr> Parser:: term() {
  auto  expr = factor();

  while (match({MINUS, PLUS})) {
    Token op = previous();
    auto right = factor();
    expr = std::make_unique<Binary>(
        std::move(expr),
        op,
        std::move(right));

  }

  return expr;
}


std::unique_ptr<Expr> Parser::factor() {
  auto expr = unary();

  while (match({SLASH, STAR})) {
    Token op = previous();
    auto right = unary();
    expr = std::make_unique<Binary>(
        std::move(expr),
        op,
        std::move(right));

  }

  return expr;
}


std::unique_ptr<Expr> Parser::  unary() {

    if (match({BANG, MINUS})) {
      Token op= previous();
      auto right = unary();
      return std::make_unique<Unary>(op,std::move(right));

    }

    return  call(); //primary();
  }

std::unique_ptr<Expr> Parser::  call() {
     auto expr = primary();

     while (true) {
       if (match({LEFT_PAREN})) {
         expr = finishCall(std::move(expr));
       } else {
         break;
       }
     }

     return expr;
   }


  std::unique_ptr<Expr> Parser::  finishCall(  std::unique_ptr<Expr>  callee) {

      std::vector<std::unique_ptr<Expr>>arguments ;

     if (!check(RIGHT_PAREN)) {
       do {
           if (arguments.size() >= 255) {
                   error(peek(), "Can't have more than 255 arguments.");
                 }
         arguments.push_back(expression());
       } while (match({COMMA}));
     }

     Token paren = consume(RIGHT_PAREN,
                           "Expect ')' after arguments.");



     return   std::make_unique<Call>(std::move(callee),paren,std::move(arguments));
   }


std::unique_ptr<Expr> Parser:: primary(){

    if(match({FALSE})) return std::make_unique<Literal>(false);
    if(match({TRUE})) return std::make_unique<Literal>(true);
    if(match({NIL})) return std::make_unique<Literal>(std::monostate{});


    if(match({NUMBER,STRING})){
        return std::make_unique<Literal>(previous().literal);
    }

    if (match({IDENTIFIER})) {
      return std::make_unique<Variable>(previous());
    }

    if(match({LEFT_PAREN})){
        auto expr=expression();
        consume(RIGHT_PAREN,"Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));


    }

throw error(peek(), "Expect expression.");

}


bool Parser::match(std::initializer_list<TokenType> types){


    for (TokenType type:types){

        if(check(type)){

            advance();
            return true;
        }
    }

    return false;
}


bool Parser::check(TokenType type){

    if(is_at_end()) return false;
    return peek().type==type;

}

Token Parser:: advance() {
   if (!is_at_end()) current++;
   return previous();
 }

bool Parser:: is_at_end() {
   return peek().type == END_OF_FILE;
 }

Token Parser::peek() {

   return tokens[current];
 }

Token Parser:: previous() {

   return tokens[current - 1];
 }



 Token Parser :: consume(TokenType type,std::string message){

     if(check(type)) return advance();

     throw error(peek(),message);

 }


  ParseError Parser::error(const Token& token,const std::string &message){

      Lox::error(token,message);
      return ParseError();

  }

void Parser::synchronize() {
    advance();

    while (!is_at_end()) {
      if (previous().type == SEMICOLON) return;

      switch (peek().type) {
        case CLASS:
        case FUN:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case PRINT:
        case RETURN:
          return;
        default:
              break;
      }

      advance();
    }
  }

//changed
std::vector<std::unique_ptr<Stmt>> Parser::parse(){

        std::vector<std::unique_ptr<Stmt>>statements;

        while(!is_at_end()){

            statements.push_back(declaration());

        }

        return statements;

  }

  std::unique_ptr<Stmt> Parser:: declaration(){

      try{


        if (match({FUN})) return function("function");
        if (match({VAR})) return varDeclaration();
        return statement();

      }catch(const ParseError &error ){
          synchronize();

          return nullptr;


      }

  }



 std::unique_ptr<Stmt>   Parser::varDeclaration(){
     Token name=consume(IDENTIFIER,"expect varible name");


    std::unique_ptr<Expr> initialzer =nullptr;
     if(match({EQUAL})){
         initialzer=expression();

     }

      consume(SEMICOLON, "Expect ';' after variable declaration.");

      return std::make_unique<Var>(name, std::move(initialzer));

  }

  //parse function
  std::unique_ptr<Stmt>  Parser:: function(std::string kind) {
    Token name = consume(IDENTIFIER, "Expect " + kind + " name.");  //consume function name

    consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");

        //parse parameters
        std::vector<Token>parameters;

        if (!check(RIGHT_PAREN)) {
          do {
            if (parameters.size() >= 255) {
              error(peek(), "Can't have more than 255 parameters.");
            }

            parameters.push_back( consume(IDENTIFIER, "Expect parameter name."));
          } while (match({COMMA}));
        }

        consume(RIGHT_PAREN, "Expect ')' after parameters.");



        consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
        //parse body
        std::vector<std::unique_ptr<Stmt>> body = block();

//block will consume rigth brace;

        return std::make_unique<Function>(std::move(name),std::move(parameters),std::move(body));


  }
std::unique_ptr<Stmt>  Parser::statement(){

if(match({PRINT})) return print_statement();
 if (match({RETURN})) return returnStatement();
 if (match({WHILE})) return whileStatement();
   if (match({FOR})) return forStatement();
   if(match({BREAK}))return break_statement();
if(match({IF})) return if_statement();
if (match({LEFT_BRACE})) return std::make_unique<Block>(block());

return expression_statement();
  }

  //return statement
  std::unique_ptr<Stmt>  Parser:: returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;

    if (!check(SEMICOLON)) {
      value = expression();
    }

    consume(SEMICOLON, "Expect ';' after return value.");

    return std::make_unique<Return>(std::move(keyword), std::move(value));
  }
  //break statment
 std::unique_ptr<Stmt> Parser:: break_statement(){
     if (loopDepth == 0) {
          error(previous(), "Cannot use 'break' outside of a loop.");
      }
      consume(SEMICOLON, "Expect ';' after loop condition.");
     // std::cout << "Parsing statement: " << peek().lexeme << "\n";

     return std::make_unique<Break>();
 }

std::unique_ptr<Stmt>  Parser:: whileStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");

    loopDepth++;
    auto body = statement();
    loopDepth--;

    return std::make_unique<While>(std::move(condition), std::move(body));
  }


  /* we do not interpret for stmt , but handle here in parser (DESUGRING)

Block(
   initializer,
    While(
      condition,
      Block(
        body,
        increment
      )
    )
  )

  */
 std::unique_ptr<Stmt>  Parser:: forStatement() {

     consume(LEFT_PAREN, "Expect '(' after 'for'.");

     // More here...
     // initializer
     std::unique_ptr<Stmt>  initializer;
      if (match({SEMICOLON})) {
        initializer = nullptr;
      } else if (match({VAR})) {
        initializer = varDeclaration();
      } else {
        initializer = expression_statement();
      }

      //condition
    std::unique_ptr<Expr> condition = nullptr;
      if (!check(SEMICOLON)) {
        condition = expression();
      }
      consume(SEMICOLON, "Expect ';' after loop condition.");

      //increament

       std::unique_ptr<Expr> increment = nullptr;
      if (!check(RIGHT_PAREN)) {
        increment = expression();
      }
      consume(RIGHT_PAREN, "Expect ')' after for clauses.");

      //
      loopDepth++;
        std::unique_ptr<Stmt>  body = statement();
        loopDepth--;


        ///
        ///

        if (increment != nullptr) {
            std::vector<std::unique_ptr<Stmt>> stmts;

            stmts.push_back(std::move(body));
            stmts.push_back(std::make_unique<Expression>(std::move(increment)));

            body = std::make_unique<Block>(std::move(stmts));
        }


        if (condition == nullptr) {
            condition = std::make_unique<Literal>(true);
        }

        body = std::make_unique<While>(
            std::move(condition),
            std::move(body)
        );


        if (initializer != nullptr) {
            std::vector<std::unique_ptr<Stmt>> stmts;

            stmts.push_back(std::move(initializer));
            stmts.push_back(std::move(body));

            body = std::make_unique<Block>(std::move(stmts));
        }


        return body;
   }


std::unique_ptr<Stmt>  Parser:: if_statement() {  //handle if condition

    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    auto  condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after if condition.");

    auto thenBranch = statement();
    std::unique_ptr<Stmt>  elseBranch = nullptr;
    if (match({ELSE})) {
      elseBranch = statement();
    }

    return std::make_unique<If>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
  }


std::unique_ptr<Stmt>  Parser::print_statement() {
     auto value = expression();                        //what  are we  getting here?
     consume(SEMICOLON, "Expect ';' after value.");
     return  std::make_unique<Print>(std::move(value));

   }



std::unique_ptr<Stmt>  Parser::expression_statement() {
     auto expr = expression();
     consume(SEMICOLON, "Expect ';' after expression.");
     return  std::make_unique<Expression>(std::move(expr));
   }


std::vector<std::unique_ptr<Stmt>> Parser::block() {
     std::vector<std::unique_ptr<Stmt>> statements ;

     while (!check(RIGHT_BRACE) && !is_at_end()) {
       statements.push_back(declaration());
     }

     consume(RIGHT_BRACE, "Expect '}' after block.");
     return statements;
   }

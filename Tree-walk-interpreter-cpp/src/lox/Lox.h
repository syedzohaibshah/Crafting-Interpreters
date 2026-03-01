#pragma once
#include<string>


#include "Scanner.h"
#include "Token.h"
#include "Expr.h"
#include "AstPrinter.h"
#include "Parser.h"
#include <memory>
#include <utility>
#include "RuntimeError.h"
#include "Interpreter.h"

class Lox{

    static const Interpreter interpreter;
    public:
    
static bool had_error=false;
static bool had_runtime_error=false;

void run_prompt();
void run_file(const std::string &path);
void run(const std::string& source);


static void error(int line,const std::string &message);
static void report(int line , const std::string &where, const std::string &message );
static void error(const Token &token, const std::string &message);
static void runtimeError(const RuntimeError & error);
};

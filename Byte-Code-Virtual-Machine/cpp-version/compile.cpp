#include <stdio.h>

#include "common.h"
#include "compile.h"
#include "scanner.h"
#include <string>
#include <string_view>
#include <iomanip>
#include <string_view>
#include<iostream>
Compiler::Compiler(const std::string& src) : source(src) {
}
void Compiler::compile(const std::string & source) {
  Scanner scanner(source);
  
  int line = -1;              
    for (;;) {
      Token token = scanner.scanToken();
      if (token.line != line) {
        std::cout<<std::setw(4)<<token.line<<" ";
        line = token.line;
      } else {
          
        std::cout << "   | ";
      }
      std::string_view lexeme(source.data() + token.start, token.length);
      std::cout<<std::setw(2)<<static_cast<int>(token.type)<<" '"<<lexeme<<"'"<<std::endl;
      if (token.type == TokenType::TOKEN_EOF) break;
    }
}
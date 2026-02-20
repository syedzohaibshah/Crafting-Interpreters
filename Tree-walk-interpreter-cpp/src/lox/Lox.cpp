
#include "Lox.h"
#include "Scanner.h"
#include "Token.h"

#include<iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include<vector>
#include "TokenType.h"

bool Lox::had_error = false;



void Lox::run_file(const std::string & path){

    std::ifstream file(path,std::ios::binary);  //read the file as raw bytes
    if(!file){
        throw std::runtime_error("could not open file "+path);
    }

    std::ostringstream buffer; //output string stream buffer
    buffer<<file.rdbuf();  //read entire file    //Copies everything from the file buffer into ostringstream buffer
    run(buffer.str());
    // Indicate an error in the exit code.
    if (had_error) std::exit(65);

}

void Lox::run_prompt(){

    std:: string line;
    while(true){

        std::cout<<"> ";
        if(!std::getline(std::cin,line)){ //read full line
            break;
        }
        run(line);
        had_error = false;
    }


}


void Lox::run(const std::string& source){

   Scanner scanner(source);
   std::vector<Token>tokens=scanner.scan_tokens();

Parser parser(tokens);
 std::unique_ptr<Expr>  expr = parser.parse();

if(had_error) return;
AstPrinter printer;
std::cout<<printer.print(*expr)<<std::endl;

}

void Lox::error(int line,const std::string &message){

    report(line,"",message);

}


void Lox:: error(const Token &token, const std::string &message) {

   if (token.type ==TokenType::END_OF_FILE) {
     report(token.line, " at end", message);
   } else {
     report(token.line, " at '" + token.lexeme + "'", message);
   }
 }


void Lox::report(int line , const std::string &where, const std::string &message ){

    std::cerr<<"[line "<<line<<"] Error"<<where<<": "<<message;
    had_error=true;

}

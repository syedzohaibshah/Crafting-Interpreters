#include"Scanner.h"
#include<utility>
#include<string.h>
#include <cassert>
#include <iostream>
#include <variant>

std::unordered_map<std::string, TokenType> Scanner::keywords = {
    {"and", AND},
    {"class", CLASS},
    {"else", ELSE},
    {"false", FALSE},
    {"for", FOR},
    {"fun", FUN},
    {"if", IF},
    {"nil", NIL},
    {"or", OR},
    {"print", PRINT},
    {"return", RETURN},
    {"super", SUPER},
    {"this", THIS},
    {"true", TRUE},
    {"var", VAR},
    {"while", WHILE}
};




Scanner::Scanner(std::string source)
:source(std::move(source)){}  // constructor


std::vector<Token> Scanner::scan_tokens(){


    while(!is_at_end()){

        // We are at the beginning of the next lexeme.
        start=current;
        scan_token();

    }
    tokens.emplace_back(TokenType::END_OF_FILE,"",std::monostate{},line);
    return tokens;

}

 bool Scanner::is_at_end(){

    return current>=source.size();
}

 void Scanner :: scan_token(){


    char c=advance();
    switch(c){
        case'(': addToken(LEFT_PAREN) ; break;
        case ')': addToken(RIGHT_PAREN); break;
        case '{': addToken(LEFT_BRACE); break;
        case '}': addToken(RIGHT_BRACE); break;
        case ',': addToken(COMMA); break;
        case '.': addToken(DOT); break;
        case '-': addToken(MINUS); break;
        case '+': addToken(PLUS); break;
        case ';': addToken(SEMICOLON); break;
        case '*': addToken(STAR); break;
        case '!':
          addToken(match('=') ? BANG_EQUAL : BANG);
          break;
        case '=':
          addToken(match('=') ? EQUAL_EQUAL : EQUAL);
          break;
        case '<':
          addToken(match('=') ? LESS_EQUAL : LESS);
          break;
        case '>':
          addToken(match('=') ? GREATER_EQUAL : GREATER);
          break;
        case '/':  //handle single line commnet
        if (match('/')) {
          // A comment goes until the end of the line.
          while (peek() != '\n' && !is_at_end()) advance();
        } else {
          addToken(SLASH);
        }
        break;


        case ' ':
        case '\r':
        case '\t':
          // Ignore whitespace.
          break;

        case '\n':  //new line
          line++;
          break;

        case '"': string(); break;   // handle when string start
        case 'o':
          if (match('r')) {
            addToken(OR);
          }
          break;




        default:

        if(is_digit(c)){
            number();
        }else if(is_alpha(c)){
            identifier();
        }
        else{
            Lox::error(line,"Unexpected character.");
        }

        break;
    }

}
//consumes the next character in the source file and returns it.
char  Scanner:: advance(){

    return source.at(current++);
}

  void  Scanner ::addToken(TokenType Type){

  addToken(Type,std::monostate{});
}
//overloading for literal
void Scanner::  addToken(TokenType Type, Object literal ){

    std::string text=source.substr(start,current-start);
    tokens.emplace_back(Type,text,literal,line);

}


bool  Scanner:: match(char expected){

    if(is_at_end()) return false;
    if(source.at(current)!=expected) return false;

    current++;
    return true;


}


 char  Scanner::peek(){

    if(is_at_end())  return '\0';

    return source.at(current);
}

void Scanner ::  string(){

    while(peek()!='"' && !is_at_end()){
        if(peek()=='\n') line++;
        advance();
    }

    if(is_at_end()){

        Lox::error(line,"unterminated string.");
        return;
    }

    // The closing ".
      advance();


     std::string value=source.substr(start+1,current-start-2);
      addToken(STRING,value);


    return ;

}


 bool Scanner:: is_digit(char c){

    return c>='0' && c<='9';

}

void Scanner::  number(){


    while(is_digit(peek()))  advance();

    if(peek()=='.' && is_digit(peek_next())){

        advance();
     while(is_digit(peek()))  advance();
    }

    addToken(NUMBER,std::stod(source.substr(start,current-start)));

}

char  Scanner::peek_next(){

    if(current+1>=source.size()) return '\0';

    return source.at(current+1);
}


void Scanner:: identifier(){
    while(is_alpha_numeric(peek())) advance();
std::string text=source.substr(start,current-start);

auto it =keywords.find(text);  //find in keywords map

TokenType Type=(it!=keywords.end())? it->second:IDENTIFIER;


addToken(Type);

}

 bool  Scanner::is_alpha(char c){

    return (c>='a' && c<='z' )||
    (c>='A' && c<='Z')||
    c=='_';

}


 bool Scanner :: is_alpha_numeric(char c){

    return is_alpha(c) || is_digit(c);
}

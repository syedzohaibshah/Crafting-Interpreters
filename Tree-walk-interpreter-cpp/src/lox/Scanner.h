#pragma once


#include<string>
#include<vector>
#include"TokenType.h"
#include"Token.h"
#include"Lox.h"
#include <unordered_map>
#include<map>
#include<variant>

class Scanner{

    const std::string source;
    std::vector<Token> tokens;
    int start=0;
    int current=0;
    int line=1;
static std::unordered_map<std::string,TokenType> keywords;

bool is_at_end();
bool match(char expected);
char advance();
char peek() ;
char peek_next();

void scan_token();
void addToken(TokenType Type, Object literal );
void addToken(TokenType Type);


void string();
bool is_digit(char c);
void number();
void identifier();
bool is_alpha(char c);
bool is_alpha_numeric(char c);

public:

explicit Scanner(std::string source);
std::vector<Token> scan_tokens();


};

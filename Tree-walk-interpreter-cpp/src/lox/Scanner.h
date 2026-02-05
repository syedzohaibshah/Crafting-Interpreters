#pragma once


#include<string>
#include<vector>
#include"Token.h"

class Scanner{

    const std::string source;
    std::vector<Token> tokens;
    int start=0;
    int current=0;
    int line=1;
std::map<string,TokenType> keywords;

    bool is_at_end() const();
    void scan_token();

void scan_token();
bool match(char expected);
void addToken(TokenType Type, Object literal );
void addToken(TokenType Type);
char advance();
char peek();
void string();
bool is_digit();
void number();
char peek_next();

public:

explicit Scanner(std::string source);

std::vector<Token> scan_tokens();

}

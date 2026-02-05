#include"Scanner.h"
#include<utility>



std::unordered_map<std::string, token_type> Scanner::keywords = {
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
    tokens.emplace_back(TokenType::END_OF_FILE,"",NULL,line));
    return tokens;

}

Scanner:: bool is_at_end(){

    return current>=source.size();
}

Scanner :: void scan_token(){


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
          while (peek() != '\n' && !isAtEnd()) advance();
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





        default:

        if(is_digit()){
            number();
        }else if(is_alpha()){
            identifier();
        }
        else{
            Lox.error(line,"Unexpected character.");
        }

        break;
    }

}
//consumes the next character in the source file and returns it.
Scanner:: char advance(){

    return source.at(current++);
}

Scanner :: void addToken(TokenType Type){

    return addToken(type,NULL);
}
//overloading for literal
Scanner:: void addToken(TokenType Type, Object literal ){

    std::string text=source.substr(start,current-start);
    tokens.emplace_back(type,text,literal,line);

}


Scanner:: bool match(char expected){

    if(is_at_end()) return false;
    if(source.at(current)!=expected) return false;

    current++;
    return true;


}


Scanner:: char peek(){

    if(is_at_end())  return '\0';

    return source.at(current);
}

Scanner :: void string(){

    while(peek()!='"' && is_at_end()){
        if(peek()=='\n') line++;
        advance();
    }

    if(is_At_end()){

        Lox.error(line,"unterminated string.");
        return;
    }

    // The closing ".
      advance();


      string value=source.substr(start+1,current-1);
      addToken(STRING,value);


    return ;

}


Scanner:: bool is_digit(char c){

    return c>='0' && c<='9';

}

Scanner:: void number(){


    while(is_digit(peek()))  advance();

    if(peek()=='.' && is_digit(peek_next())){

        advance();
     while(is_digit(peek()))  advance();
    }

    add_token(NUMBER,std::stod(source.substr(start,current)));

}

Scanner char peek_next(){

    if(current+1>=source.size()) return '\0';

    return source.at(current+1);
}


Scanner::void identifier(){
    while(is_alpha_numeric(peek())) advance();

    addToken(IDENTIFIER);

}

Scanner:: bool is_alpha(char c){

    return c>='a' && c<='z' ||
    c>='A' && c<='Z'||
    c=='_';

}


Scanner :: bool is_alpha_numeric(char c){

    return is_alpha() || is_digit();
}

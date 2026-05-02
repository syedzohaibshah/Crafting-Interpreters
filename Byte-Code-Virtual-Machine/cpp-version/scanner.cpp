
#include "scanner.h"
#include <cstring>
#include <iostream>
#include <stdexcept>


Scanner:: Scanner( std::string source)
:source(std::move(source)),source_view(this->source),
start(0),current(0),line(1){};



bool Scanner::  isAtEnd() const {
  return current >= source.length();
}

char Scanner::  advance() {
  current++;
  return source[current-1];
}
 char Scanner:: peek() const  {
  return source[current];
}

bool  Scanner:: match(char expected) {
  if (isAtEnd()) return false;
  if (source[current] != expected) return false;
  current++;
  return true;
}
char Scanner::  peekNext()const {
  if (current + 1 >= source.length()) return '\0';
  return source[current+1];
}

 Token Scanner:: makeToken(TokenType type) const {
  Token token;
  token.type = type;
  token.start = start;
  token.length = (int)(current - start);
  token.line = line;
  return token;
}

  Token Scanner:: errorToken(std::string_view message) const {
  Token token;
  token.type = TokenType::TOKEN_ERROR;
  token.start = current;
  token.length = (int)message.length();
  token.line = line;
  return token;
}


  void  Scanner::skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
      case '\n':
         line++;
          advance();
          break;
      case '/':
            if (peekNext() == '/') {
              // A comment goes until the end of the line.
              while (peek() != '\n' && !isAtEnd()) advance();
            } else {
              return;
            }
            break;

      default:
        return;
    }
  }
}

 Token Scanner:: stringToken() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') line++;
    advance();
  }

  if (isAtEnd()) return errorToken("Unterminated string.");

  // The closing quote.
  advance();
  return makeToken(TokenType::TOKEN_STRING);
}

  bool Scanner:: isDigit(char c)const{
  return c >= '0' && c <= '9';
}
bool Scanner::isAlpha(char c) const{
 return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
         c == '_';
}
 Token  Scanner:: number() {
  while (isDigit(peek())) advance();

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the ".".
    advance();

    while (isDigit(peek())) advance();
  }

  return makeToken(TokenType::TOKEN_NUMBER);
}



TokenType Scanner::checkKeyword(size_t kw_start, size_t kw_length,
                                 std::string_view rest, TokenType type) const {
  // Check if the identifier length matches start + length
  if (current - start == kw_start + kw_length) {
    // Extract the substring from source and compare with rest
    std::string_view lexeme = std::string_view(source).substr(start + kw_start, kw_length);
    if (lexeme == rest) {
      return type;
    }
  }
  return TokenType::TOKEN_IDENTIFIER;
}



TokenType Scanner::  identifierType() const{
  
    switch (source[start]) {
      case 'a': return checkKeyword(1, 2, "nd", TokenType::TOKEN_AND);
      case 'c': return checkKeyword(1, 4, "lass", TokenType::TOKEN_CLASS);
      case 'e': return checkKeyword(1, 3, "lse", TokenType::TOKEN_ELSE);
      case 'f':
        if (current - start > 1) {
          switch (source[start + 1]) {
            case 'a': return checkKeyword(2, 3, "lse", TokenType::TOKEN_FALSE);
            case 'o': return checkKeyword(2, 1, "r", TokenType::TOKEN_FOR);
            case 'u': return checkKeyword(2, 1, "n", TokenType::TOKEN_FUN);
          }
        }
        break;
      case 'i': return checkKeyword(1, 1, "f", TokenType::TOKEN_IF);
      case 'n': return checkKeyword(1, 2, "il", TokenType::TOKEN_NIL);
      case 'o': return checkKeyword(1, 1, "r", TokenType::TOKEN_OR);
      case 'p': return checkKeyword(1, 4, "rint", TokenType::TOKEN_PRINT);
      case 'r': return checkKeyword(1, 5, "eturn", TokenType::TOKEN_RETURN);
      case 's': return checkKeyword(1, 4, "uper", TokenType::TOKEN_SUPER);
      case 'v': return checkKeyword(1, 2, "ar", TokenType::TOKEN_VAR);
      case 'w': return checkKeyword(1, 4, "hile", TokenType::TOKEN_WHILE);
      case 't':
        if (current -start > 1) {
          switch (source[start+1]) {
            case 'h': return checkKeyword(2, 2, "is", TokenType::TOKEN_THIS);
            case 'r': return checkKeyword(2, 2, "ue", TokenType::TOKEN_TRUE);
          }
        }
        break;
    }
  return TokenType::TOKEN_IDENTIFIER;
}
 Token  Scanner:: identifier() {
  while (isAlpha(peek()) || isDigit(peek())) advance();
  return makeToken(identifierType());
}
Token  Scanner:: scanToken() {
      skipWhitespace();
  start = current;

  if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);

  char c = advance();
if (isAlpha(c)) return identifier();
 if (isDigit(c)) return number();
  switch (c) {
    case '(': return makeToken(TokenType::TOKEN_LEFT_PAREN);
    case ')': return makeToken(TokenType::TOKEN_RIGHT_PAREN);
    case '{': return makeToken(TokenType::TOKEN_LEFT_BRACE);
    case '}': return makeToken(TokenType::TOKEN_RIGHT_BRACE);
    case ';': return makeToken(TokenType::TOKEN_SEMICOLON);
    case ',': return makeToken(TokenType::TOKEN_COMMA);
    case '.': return makeToken(TokenType::TOKEN_DOT);
    case '-': return makeToken(TokenType::TOKEN_MINUS);
    case '+': return makeToken(TokenType::TOKEN_PLUS);
    case '/': return makeToken(TokenType::TOKEN_SLASH);
    case '*': return makeToken(TokenType::TOKEN_STAR);
    case '!':
      return makeToken(
          match('=') ? TokenType::TOKEN_BANG_EQUAL : TokenType::TOKEN_BANG);
    case '=':
      return makeToken(
          match('=') ? TokenType::TOKEN_EQUAL_EQUAL : TokenType::TOKEN_EQUAL);
    case '<':
      return makeToken(
          match('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS);
    case '>':
      return makeToken(
          match('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER);
    case '"': return stringToken();
  }

  return errorToken("Unexpected character.");

}

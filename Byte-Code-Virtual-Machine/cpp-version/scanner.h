#ifndef CLOX_SCANNER_H
#define CLOX_SCANNER_H

#include <string>
#include <string_view>
#include "Token.h"

class Scanner {
public:
  explicit Scanner(std::string source);
  Token scanToken();

private:
  std::string source;
  std::string_view source_view;
  size_t start = 0;
  size_t current= 0;
  int line = 1;

  bool isAtEnd() const;
  Token makeToken(TokenType type) const;
  Token errorToken(std::string_view message) const;

  char advance();
  char peek() const;
  char peekNext() const;
  bool match(char expected);

  void skipWhitespace();
  Token stringToken();
  Token number();
  Token identifier();

  bool isDigit(char c) const;
  bool isAlpha(char c) const;
  TokenType identifierType() const;

  TokenType checkKeyword(
      size_t start,
      size_t length,
      std::string_view rest,
      TokenType type) const;
};

#endif
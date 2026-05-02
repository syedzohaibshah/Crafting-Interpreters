#ifndef clox_compiler_h
#define clox_compiler_h

#include <string>
class Compiler {
  std::string source;
public:
  Compiler(const std::string& src);
  void compile(const std::string& source);
};

#endif
// vm.h
#ifndef CLOX_VM_H
#define CLOX_VM_H

#include <vector>
#include <memory>
#include "chunk.h"
#include "compile.h"

enum class InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class VM {
public:
  VM();
  ~VM() = default;

  
InterpretResult interpret(const std::string &source);

private:
  InterpretResult run();
  void push(Value value);
  Value pop();
  void resetStack();

  std::unique_ptr<Chunk> chunk;
   uint8_t* ip = nullptr;
  std::vector<Value> stack;
};

#endif
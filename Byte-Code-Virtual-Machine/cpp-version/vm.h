// vm.h
#ifndef CLOX_VM_H
#define CLOX_VM_H

#include <vector>
#include <memory>
#include "chunk.h"

enum class InterpretResult {
  OK,
  COMPILE_ERROR,
  RUNTIME_ERROR
};

class VM {
public:
  VM();
  ~VM() = default;

  InterpretResult interpret(const Chunk& chunk);

private:
  InterpretResult run();
  void push(Value value);
  Value pop();
  void resetStack();

  const Chunk* chunk = nullptr;
  const uint8_t* ip = nullptr;
  std::vector<Value> stack;
};

#endif
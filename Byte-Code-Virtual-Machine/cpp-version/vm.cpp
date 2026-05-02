#include "vm.h"
#include "debug.h"
#include <iostream>
#include <cstddef>
#include "compile.h"

constexpr size_t STACK_MAX = 256;

VM::VM() {
  stack.reserve(STACK_MAX);
}

void VM::resetStack() {
  stack.clear();
}

void VM::push(Value value) {
  stack.push_back(value);
}

Value VM::pop() {
  Value value = stack.back();
  stack.pop_back();
  return value;
}

InterpretResult VM::interpret(const std::string &source) {
  Compiler compiler(source);
  compiler.compile(source);
  return InterpretResult::INTERPRET_OK;

}


InterpretResult VM::run() {
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->constants[READ_BYTE()])
#define BINARY_OP(op) \
  do { \
    double b = pop(); \
    double a = pop(); \
    push(a op b); \
  } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    std::cout << "          ";
    for (const auto& value : stack) {
      std::cout << "[ ";
      printValue(value);
      std::cout << " ]";
    }
    std::cout << "\n";
    disassembleInstruction(*chunk, static_cast<int>(ip - chunk->code.data()));
#endif

    uint8_t instruction = READ_BYTE();
    switch (instruction) {
      case static_cast<uint8_t>(OpCode::OP_CONSTANT): {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }
      case static_cast<uint8_t>(OpCode::OP_NEGATE):
        push(-pop());
        break;
      case static_cast<uint8_t>(OpCode::OP_ADD):
        BINARY_OP(+);
        break;
      case static_cast<uint8_t>(OpCode::OP_SUBTRACT):
        BINARY_OP(-);
        break;
      case static_cast<uint8_t>(OpCode::OP_MULTIPLY):
        BINARY_OP(*);
        break;
      case static_cast<uint8_t>(OpCode::OP_DIVIDE):
        BINARY_OP(/);
        break;
      case static_cast<uint8_t>(OpCode::OP_RETURN): {
        printValue(pop());
        std::cout << "\n";
        return InterpretResult::INTERPRET_OK;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

#include "vm.h"
#include "debug.h"
#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <stdarg.h>
#include "chunk.h"

constexpr size_t STACK_MAX = 256;

VM::VM() {
  stack.reserve(STACK_MAX);
}

void VM::resetStack() {
  stack.clear();
}
//this part
void VM:: runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = ip - chunk->code.data() - 1;
  int line = chunk->lines[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack();
}

void VM::push(Value value) {
  stack.push_back(value);
}

Value VM::pop() {
  Value value = stack.back();
  stack.pop_back();
  return value;
}

Value VM::peek(int distance) {
  return stack[stack.size()-distance-1]; //correct the error
}

bool VM:: isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

InterpretResult VM::interpret(const std::string &source) {

   Compiler compiler(source);


   auto chunk=std::make_unique<Chunk>();
   if (!compiler.compile(chunk.get())) {
     return InterpretResult::INTERPRET_COMPILE_ERROR;
   }
   this->chunk = std::move(chunk);
   this->ip = this->chunk->code.data(); //code is vector

   InterpretResult result = run();

  return result;

}


InterpretResult VM::run() {
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->constants[READ_BYTE()])
#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        runtimeError("Operands must be numbers."); \
        return InterpretResult::INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
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
      if (!IS_NUMBER(peek(0))) {  //double check peek(0)
        std::runtime_error("Operand must be a number.");
        return InterpretResult::INTERPRET_RUNTIME_ERROR;
      }
    push(NUMBER_VAL(-AS_NUMBER(pop())));
        break;
      case static_cast<uint8_t>(OpCode::OP_ADD):
       BINARY_OP(NUMBER_VAL, +); break;

      case static_cast<uint8_t>(OpCode::OP_SUBTRACT):
        BINARY_OP(NUMBER_VAL, -); break;

      case static_cast<uint8_t>(OpCode::OP_MULTIPLY):
       BINARY_OP(NUMBER_VAL, *); break;
      case static_cast<uint8_t>(OpCode::OP_DIVIDE):
        BINARY_OP(NUMBER_VAL, /); break;

      case static_cast<uint8_t>(OpCode::OP_NIL):push(NIL_VAL); break;
      case static_cast<uint8_t>(OpCode::OP_TRUE): push(BOOL_VAL(true)); break;
      case static_cast<uint8_t>(OpCode::OP_FALSE): push(BOOL_VAL(false)); break;

      case static_cast<uint8_t>(OpCode::OP_NOT):
        push(BOOL_VAL(isFalsey(pop())));
        break;

      case static_cast<uint8_t>(OpCode::OP_EQUAL): {
          Value b = pop();
          Value a = pop();
          push(BOOL_VAL(valuesEqual(a, b)));
          break;
        }
        case static_cast<uint8_t>(OpCode::OP_GREATER):  BINARY_OP(BOOL_VAL, >); break;
        case static_cast<uint8_t>(OpCode::OP_LESS):     BINARY_OP(BOOL_VAL, <); break;


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

#include "vm.h"
#include "debug.h"
#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <stdarg.h>
#include "chunk.h"
#include <string.h>
#include "object.h"


constexpr size_t STACK_MAX = 256;

VM::VM() {
  stack.reserve(STACK_MAX);
    objects = nullptr;
    initTable(&strings);
    initTable(&globals);
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

 void  VM::concatenate() {
  ObjString* b = AS_STRING(pop());
  ObjString* a = AS_STRING(pop());

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString* result = takeString(chars, length);
  push(OBJ_VAL(result));
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
#define READ_STRING() AS_STRING(READ_CONSTANT())
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
      case static_cast<uint8_t>(OpCode::OP_ADD): {
        if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
          concatenate();
        } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
          double b = AS_NUMBER(pop());
          double a = AS_NUMBER(pop());
          push(NUMBER_VAL(a + b));
        } else {
          runtimeError(
              "Operands must be two numbers or two strings.");
          return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        break;
      } 

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
        case static_cast<uint8_t>(OpCode::OP_PRINT): {
          printValue(pop());
          std::cout<<"\n";
          break;
        }
         case static_cast<uint8_t>(OpCode::OP_POP): pop(); break;
         case static_cast<uint8_t>(OpCode::OP_DEFINE_GLOBAL): {
           ObjString* name = READ_STRING();
           tableSet(&vm.globals, name, peek(0));
           pop();
           break;
         }
         case static_cast<uint8_t>(OpCode::OP_GET_GLOBAL): {
           ObjString* name = READ_STRING();
           Value value;
           if (!tableGet(&vm.globals, name, &value)) {
             runtimeError("Undefined variable '%s'.", name->chars);
             return InterpretResult::INTERPRET_RUNTIME_ERROR;
           }
           push(value);
           break;
         }
    


      case static_cast<uint8_t>(OpCode::OP_RETURN): {
        // printValue(pop());
        // std::cout << "\n";
        return InterpretResult::INTERPRET_OK;
      }

    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
#undef READ_STRING
}
//freeing memory
static void freeObject(Obj* object) {
  switch (object->type) {
    case OBJ_STRING: {
      ObjString* string = (ObjString*)object;
      // Characters are stored inline, so only free the ObjString itself
      FREE(ObjString, object);
      break;
    }
  }
}

void VM::freeObjects() {
  Obj* object = objects;
  while (object != NULL) {
    Obj* next = object->next;
    freeObject(object);
    object = next;
  }
}

VM::~VM(){
  freeObjects();
  freeTable(&vm.strings);
    freeTable(&vm.globals);
}

VM vm;

// debug.cpp (updated)
#include "debug.h"
#include "chunk.h"
#include "object.h"
#include <iostream>
#include <cstdio>
#include <string.h>
#include "object.h"

void printValue(Value value) {
         if (std::holds_alternative<bool>(value))
             std::cout<<AS_BOOL(value) ? "true" : "false";
         
        else if (std::holds_alternative<std::nullptr_t>(value)) std::cout<<"nil"; 
        else if (std::holds_alternative<double>(value))  std::cout<<AS_NUMBER(value); 
        else if (std::holds_alternative<Obj*>(value)) printObject(value);

}

bool valuesEqual(Value a, Value b) {
  if (a != b) return false;
  if (std::holds_alternative<bool>(a))
    return AS_BOOL(a) == AS_BOOL(b);
    
  else if (std::holds_alternative<std::nullptr_t>(a))   
      return true;
  
  else if (std::holds_alternative<double>(a)) 
    return AS_NUMBER(a) == AS_NUMBER(b);
    
  else if(std::holds_alternative<Obj*>(a)) 
      return AS_OBJ(a) == AS_OBJ(b);
  
  else   return false; // Unreachable.
  
}

void disassembleChunk(const Chunk& chunk, const char* name) {
  std::printf("== %s ==\n", name);
  for (int offset = 0; offset < static_cast<int>(chunk.code.size());) {
    offset = disassembleInstruction(chunk, offset);
  }
}

static int constantInstruction(const char* name, const Chunk& chunk, int offset) {
  uint8_t constant = chunk.code[offset + 1];
  std::printf("%-16s %4d '", name, constant);
  printValue(chunk.constants[constant]);
  std::printf("'\n");
  return offset + 2;
}

static int simpleInstruction(const char* name, int offset) {
  std::printf("%s\n", name);
  return offset + 1;
}

static int byteInstruction(const char* name, const  Chunk* chunk,
                           int offset) {
  uint8_t slot = chunk->code[offset + 1];
  printf("%-16s %4d\n", name, slot);
  return offset + 2; 
}

static int longConstantInstruction(const Chunk& chunk, int offset) {
  uint8_t byte1 = chunk.code[offset + 1];
  uint8_t byte2 = chunk.code[offset + 2];
  uint8_t byte3 = chunk.code[offset + 3];
  int constant = (byte1 << 16) | (byte2 << 8) | byte3;

  std::printf("%-16s %5d '", "OP_CONSTANT_LONG", constant);
  printValue(chunk.constants[constant]);
  std::printf("'\n");
  return offset + 4;
}
static int jumpInstruction(const char* name, int sign,
                           const Chunk* chunk, int offset) {
  uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
  jump |= chunk->code[offset + 2];
  printf("%-16s %4d -> %d\n", name, offset,
         offset + 3 + sign * jump);
  return offset + 3;
}

int disassembleInstruction(const Chunk& chunk, int offset) {
  std::printf("%04d ", offset);

  if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1]) {
    std::printf("   | ");
  } else {
    std::printf("%4d ", chunk.lines[offset]);
  }

  uint8_t instruction = chunk.code[offset];
  switch (instruction) {
    case static_cast<uint8_t>(OpCode::OP_RETURN):
      return simpleInstruction("OP_RETURN", offset);
      
    case static_cast<uint8_t>(OpCode::OP_CONSTANT):
      return constantInstruction("OP_CONSTANT", chunk, offset);
      
    case static_cast<uint8_t>(OpCode::OP_CONSTANT_LONG):
      return longConstantInstruction(chunk, offset);
      
    case static_cast<uint8_t>(OpCode::OP_NEGATE):
      return simpleInstruction("OP_NEGATE", offset);
      
    case static_cast<uint8_t>(OpCode::OP_ADD):
      return simpleInstruction("OP_ADD", offset);
      
    case static_cast<uint8_t>(OpCode::OP_SUBTRACT):
      return simpleInstruction("OP_SUBTRACT", offset);
      
    case static_cast<uint8_t>(OpCode::OP_MULTIPLY):
      return simpleInstruction("OP_MULTIPLY", offset);
      
    case static_cast<uint8_t>(OpCode::OP_DIVIDE):
      return simpleInstruction("OP_DIVIDE", offset);
      
      case static_cast<uint8_t>(OpCode::OP_NIL):
        return simpleInstruction("OP_NIL", offset);
        
      case static_cast<uint8_t>(OpCode::OP_TRUE):
        return simpleInstruction("OP_TRUE", offset);
        
      case static_cast<uint8_t>(OpCode::OP_FALSE):
        return simpleInstruction("OP_FALSE", offset);

        case static_cast<uint8_t>(OpCode::OP_NOT):
          return simpleInstruction("OP_NOT", offset);
          case static_cast<uint8_t>(OpCode::OP_EQUAL):
            return simpleInstruction("OP_EQUAL", offset);
          case static_cast<uint8_t>(OpCode::OP_GREATER):
            return simpleInstruction("OP_GREATER", offset);
          case static_cast<uint8_t>(OpCode::OP_LESS):
            return simpleInstruction("OP_LESS", offset);
            
          case static_cast<uint8_t>(OpCode::OP_PRINT):
              return simpleInstruction("OP_PRINT", offset);
          case static_cast<uint8_t>(OpCode::OP_POP):
                return simpleInstruction("OP_POP", offset);
        case static_cast<uint8_t>(OpCode::OP_DEFINE_GLOBAL):
                  return constantInstruction("OP_DEFINE_GLOBAL", chunk,
                                             offset);
        case static_cast<uint8_t>(OpCode::OP_GET_GLOBAL):
                    return constantInstruction("OP_GET_GLOBAL", chunk, offset);

        case  static_cast<uint8_t>(OpCode::OP_SET_GLOBAL):
                      return constantInstruction("OP_SET_GLOBAL", chunk, offset);
            case static_cast<uint8_t>(OpCode::OP_GET_LOCAL):
                        return byteInstruction("OP_GET_LOCAL", &chunk, offset);
            case static_cast<uint8_t>(OpCode::OP_SET_LOCAL):
                        return byteInstruction("OP_SET_LOCAL", &chunk, offset);
         case static_cast<uint8_t>(OpCode::OP_JUMP):
                          return jumpInstruction("OP_JUMP", 1, &chunk, offset);
        case static_cast<uint8_t>(OpCode::OP_JUMP_IF_FALSE):
                          return jumpInstruction("OP_JUMP_IF_FALSE", 1, &chunk, offset);
        case static_cast<uint8_t>(OpCode::OP_LOOP):
                return jumpInstruction("OP_LOOP", -1, &chunk, offset);
      
    default:
      std::printf("Unknown opcode %d\n", instruction);
      return offset + 1;
  }
}
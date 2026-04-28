// debug.cpp (updated)
#include "debug.h"
#include "chunk.h"

#include <iostream>
#include <cstdio>

void printValue(Value value) {
  std::cout << value;
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
      
    default:
      std::printf("Unknown opcode %d\n", instruction);
      return offset + 1;
  }
}
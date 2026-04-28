#include "chunk.h"
#include "vm.h"
#include "debug.h"

int main() {
  Chunk chunk;
  
  // Use writeConstant() which handles opcode + index
  chunk.writeConstant(1.2, 1);
  chunk.writeConstant(3.4, 1);
  chunk.writeChunk(static_cast<uint8_t>(OpCode::OP_ADD), 1);
  chunk.writeChunk(static_cast<uint8_t>(OpCode::OP_RETURN), 1);

  disassembleChunk(chunk, "test chunk");

  VM vm;
  vm.interpret(chunk);

  return 0;
}
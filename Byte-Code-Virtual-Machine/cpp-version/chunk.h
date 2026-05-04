#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"

#include<vector>
using Value = double;

typedef enum {
  OP_RETURN,
  OP_CONSTANT,
  OP_CONSTANT_LONG,
   OP_NEGATE,
   OP_ADD,
   OP_SUBTRACT,
   OP_MULTIPLY,
   OP_DIVIDE,
} OpCode;



class Chunk{

   public :
   Chunk() = default;
  ~Chunk() = default;

  void writeChunk(uint8_t byte, int line);
  int addConstant(const Value& value);
  void writeConstant(const Value& value, int line);
 
   // Data members
  std::vector<uint8_t> code;
  std::vector<int> lines;
  std::vector<Value> constants;  // Use vector instead of ValueArray

private:
  // No public count - use code.size() instead

};


#endif

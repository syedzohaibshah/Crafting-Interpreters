#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include <variant>
#include<vector>

typedef struct Obj Obj;
typedef struct ObjString ObjString;


#define BOOL_VAL(value)   (Value(value))
#define NIL_VAL           (Value(nullptr))
#define NUMBER_VAL(value) (Value(value))
#define OBJ_VAL(object)   (Value(value))


#define AS_BOOL(value)    (std::get<bool>(value))
#define AS_NUMBER(value)  (std::get<double>(value))
#define AS_OBJ(value)     (std::get<Obj*>(value))

#define IS_BOOL(value)    (std::holds_alternative<bool>(value))
#define IS_NIL(value)     (std::holds_alternative<std::nullptr_t>(value))
#define IS_NUMBER(value)  (std::holds_alternative<double>(value))
#define IS_OBJ(value)     (std::holds_alternative<Obj*>(value))


using Value = std::variant<std::nullptr_t, bool, double,Obj* >;  // nil, bool, number


//using Value = double;

typedef enum {
  OP_RETURN,
  OP_CONSTANT,
  OP_CONSTANT_LONG,
   OP_NEGATE,
   OP_ADD,
   OP_SUBTRACT,
   OP_MULTIPLY,
   OP_DIVIDE,
   OP_NIL,
   OP_TRUE,
   OP_FALSE,
   OP_NOT,
   OP_EQUAL,
   OP_GREATER,
   OP_LESS,
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

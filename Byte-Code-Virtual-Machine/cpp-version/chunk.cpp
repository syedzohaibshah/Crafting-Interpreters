#include "chunk.h"


void Chunk::writeChunk(uint8_t byte,int line) {

  code.push_back(byte);
 lines.push_back(line);

}



int Chunk::addConstant(const Value& value) {
  constants.push_back(value);
  return constants.size() - 1;
}


void Chunk::writeConstant(const Value& value, int line){
    int index = addConstant( value);

    if(index<256){
        writeChunk(OpCode::OP_CONSTANT,line);
        writeChunk((uint8_t)index,line);

    }else{
         writeChunk(OpCode::OP_CONSTANT, line);
         writeChunk((uint8_t)((index>>16)&0xFF), line);
         writeChunk((uint8_t)((index>>8)&0xFF), line);
         writeChunk((uint8_t)((index) & 0xFF), line);
    }



}

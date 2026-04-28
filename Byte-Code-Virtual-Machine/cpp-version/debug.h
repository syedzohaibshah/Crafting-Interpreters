// debug.h
#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H
#include"chunk.h"
class Chunk;

void disassembleChunk(const Chunk& chunk, const char* name);
int disassembleInstruction(const Chunk& chunk, int offset);
void printValue(Value value);
#endif
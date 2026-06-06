#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "chunk.h"
#include "object.h"

#include <unordered_map>


// Custom hash function for ObjString* keys
struct ObjStringHash {
  size_t operator()(ObjString* key) const {
    return key->hash;
  }
};

// Custom equality comparator for ObjString* keys
struct ObjStringEqual {
  bool operator()(ObjString* a, ObjString* b) const {
    return a == b; // Pointer equality (strings are interned)
  }
};

using Table = std::unordered_map<ObjString*, Value, ObjStringHash, ObjStringEqual>;

void initTable(Table* table);
void freeTable(Table* table);
bool tableSet(Table* table, ObjString* key, Value value);
void tableAddAll(Table* from, Table* to);
bool tableGet(Table* table, ObjString* key, Value* value);
bool tableDelete(Table* table, ObjString* key);
ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash);
#endif
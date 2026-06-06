#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"

void initTable(Table* table) {
  table->clear();
}

void freeTable(Table* table) {
  table->clear();
}

bool tableSet(Table* table, ObjString* key, Value value) {
  bool isNewKey = table->find(key) == table->end();
  (*table)[key] = value;
  return isNewKey;
}

void tableAddAll(Table* from, Table* to) {
  for (auto& [key, value] : *from) {
    tableSet(to, key, value);
  }
}

bool tableGet(Table* table, ObjString* key, Value* value) {
  auto it = table->find(key);
  if (it != table->end()) {
    *value = it->second;
    return true;
  }
  return false;
}

bool tableDelete(Table* table, ObjString* key) {
  auto it = table->find(key);
  if (it != table->end()) {
    table->erase(it);
    return true;
  }
  return false;
}

ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash) {
  for (auto& [key, value] : *table) {
    if (key->length == length &&
        key->hash == hash &&
        memcmp(key->chars, chars, length) == 0) {
      return key;
    }
  }
  return NULL;
}


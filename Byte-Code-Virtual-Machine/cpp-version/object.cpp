#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "chunk.h"
#include "vm.h"
#include "table.h"

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  //
  object->next = vm.objects;
  vm.objects = object;
  //
  
  return object;
}
#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

    static ObjString* allocateString(const char* chars, int length, uint32_t hash) {
      // Allocate ObjString and character array in single contiguous block
      size_t size = sizeof(ObjString) + length + 1;
      ObjString* string = (ObjString*)allocateObject(size, OBJ_STRING);
      string->length = length;
      memcpy(string->chars, chars, length);
      string->chars[length] = '\0';
      string->hash = hash;
      tableSet(&vm.strings, string, NIL_VAL);
      return string;
    }

    void printObject(Value value) {
      switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
          printf("%s", AS_CSTRING(value));
          break;
      }
    }
    static uint32_t hashString(const char* key, int length) {
      uint32_t hash = 2166136261u;
      for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
      }
      return hash;
    }

ObjString* copyString(const char* chars, int length) {
  // Single allocation - no need for temporary heap allocation
  uint32_t hash = hashString(chars, length);
  
  ObjString* interned = tableFindString(&vm.strings, chars, length,
                                        hash);
  if (interned != NULL) return interned;
  return allocateString(chars, length,hash);
}

ObjString* takeString(char* chars, int length) {
     uint32_t hash = hashString(chars, length);
     ObjString* interned = tableFindString(&vm.strings, chars, length,
                                           hash);
     if (interned != NULL) {
       FREE_ARRAY(char, chars, length + 1);
       return interned;
     }
  return allocateString(chars, length,hash);
}

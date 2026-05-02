#include "chunk.h"
#include "vm.h"
#include "debug.h"
#include<iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdexcept>


static std::string readFile(const std::string &path) {
  
  std::ifstream file(path,std::ios::binary);  //read the file as raw bytes
  if(!file){
      throw std::runtime_error("could not open file "  +path);
  }


  file.seekg(0, std::ios::end);
 auto size = file.tellg();
 if (size < 0) {
        throw std::runtime_error("failed to determine file size: " + path);
    }
  file.seekg(0, std::ios::beg);
  
  std::string buffer(static_cast<size_t>(size), '\0');

file.read(&buffer[0], size);

if (!file) {
     throw std::runtime_error("could not read file " + path);
 }

 
  return buffer;
}


static int runFile(VM& vm,const std::string &path) {
  std::string source = readFile(path);
  InterpretResult result = vm.interpret(source);
 

  if (result == InterpretResult::INTERPRET_COMPILE_ERROR) return 65;
  if (result == InterpretResult::INTERPRET_RUNTIME_ERROR) return 70;
return 0;
}

static void repl(VM& vm) {
  
  std:: string line;
  while(true){

      std::cout<<"> ";
      if(!std::getline(std::cin,line)){ //read full line
          break;
      }
        vm.interpret(line);
     
      
  }
}

int main(int argc, const char* argv[]) {

    VM vm;
   
    if (argc == 1) {
      repl(vm);
    } else if (argc == 2) {
        try {
               return runFile(vm, argv[1]);
           } catch (const std::exception& e) {
               std::cerr << e.what() << "\n";
               return 70;
           }
      runFile( vm,argv[1]);
    } 
 std::cerr << "Usage: clox [path]\n";
  // Chunk chunk;
  
  // // Use writeConstant() which handles opcode + index
  // chunk.writeConstant(2, 1);
  // chunk.writeConstant(3, 1);
  // chunk.writeChunk(static_cast<uint8_t>(OpCode::OP_MULTIPLY), 1);

  // chunk.writeConstant(4, 1);
  
  // chunk.writeConstant(5, 1);
  // chunk.writeChunk(static_cast<uint8_t>(OpCode::OP_NEGATE), 1);
  
  
  // chunk.writeChunk(static_cast<uint8_t>(OpCode::OP_DIVIDE), 1);
  
  //  chunk.writeChunk(static_cast<uint8_t>(OpCode::OP_SUBTRACT), 1);
   
  //   chunk.writeConstant(1, 1);
  
  // chunk.writeChunk(static_cast<uint8_t>(OpCode::OP_ADD), 1);
  // chunk.writeChunk(static_cast<uint8_t>(OpCode::OP_RETURN), 1);

  // disassembleChunk(chunk, "test chunk");



  return 0;
}
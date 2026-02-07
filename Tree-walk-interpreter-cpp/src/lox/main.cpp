#include<iostream>
#include<cstdlib>
#include "Lox.h"


int main(int argc, char* argv[]){
Lox lox;
    if(argc>2){
        std::cout<<"Usage clox [script]\n";
        exit(64);


    }else if(argc==2){

        lox.run_file(argv[1]);

    }else{

        lox.run_prompt();
    }


}

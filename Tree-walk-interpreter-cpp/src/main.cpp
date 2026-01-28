#include<iostream>
#include<cstdlib>
#include"lox.hpp"

using namespace std;


int main(int argc, char* argv[]){

    if(argc>2){
        cout<<"Usage clox [script]\n";
        exit(64);


    }else if(argc==2){

        Lox::run_file(argv[1]);
    }else{

        Lox::run_promtp();
    }


}

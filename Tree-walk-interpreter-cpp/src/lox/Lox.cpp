#include<iostream>

#include "Lox.hpp"







void Lox::run_file(const std::string& path){

    std:ifstream file(path,std::ios::binary);  //read the file as raw bytes
    if(!file){
        throw std::runtime_error("could not open file "+path);
    }

    std::ostringstream buffer; //output string stream buffer
    buffer<<file.rdbuf();  //read entire file    //Copies everything from the file buffer into ostringstream buffer
    run(buffer.str());

}

void Lox::run_promt(){

    std: string line;
    while(true){

        std::cout<<"> ";
        if(!std::getline(std::cin,line)){ read full line
            break;
        }
        run(line);
    }


}


void Lox::run(const std::string& source){

   Scanner scanner(source);
   std::vector<Token>tokens=scanner.scan_tokens();


    for(const Token& token: tokens ){

        std::cout<<token<<"\n";
    }


}

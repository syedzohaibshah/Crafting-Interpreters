// #include<iostream>
// #include<cstdlib>
// #include "Lox.h"


// int main(int argc, char* argv[]){
// Lox lox;
//     if(argc>2){
//         std::cout<<"Usage clox [script]\n";
//         exit(64);


//     }else if(argc==2){

//         lox.run_file(argv[1]);

//     }else{

//         lox.run_prompt();
//     }


// }

// #include <iostream>
// #include <memory>
// #include "AstPrinter.h"

// // Builds the tree: (* (- 123) (group 45.67))
// int main() {
//     auto expression = std::make_unique<Binary>(
//         std::make_unique<Unary>(
//             Token(MINUS, "-", std::monostate{}, 1),
//             std::make_unique<Literal>(Object{123.0})),
//         Token(STAR, "*", std::monostate{}, 1),
//         std::make_unique<Grouping>(
//             std::make_unique<Literal>(Object{45.67}))
//     );

//     AstPrinter printer;
//     std::cout << printer.print(*expression) << "\n";
// }
import math_utils;   // import module

#include <iostream>

int main() {
    std::cout << add(3, 4) << "\n";
    std::cout << square(5) << "\n";
}


//g++-15 -std=c++20 -fmodules-ts -c math_utils.cppm

//g++-15 -std=c++20 -fmodules-ts module.cpp math_utils.o -o m

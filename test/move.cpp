#include <iostream>
#include <string>
#include <utility>  // for std::move

int main() {
    std::string a = "Hello";
    std::string b = std::move(a);

    std::cout << "a: " << a << "\n";
    std::cout << "b: " << b << "\n";
}

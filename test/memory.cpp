#include <iostream>
int main() {
    int* ptr = new int(42);  // Memory leak - never deleted
    std::cout << *ptr << std::endl;
    // ptr is never deleted
    return 0;
}
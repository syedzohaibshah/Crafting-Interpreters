#include <memory>
#include <iostream>
#include<any>
// int main() {
//     std::unique_ptr<int> p1 = std::make_unique<int>(10);

//     std::cout << *p1 << std::endl;   // 10

//     // std::unique_ptr<int> p2 = p1;// ❌ NOT allowed (copy)

//     std::unique_ptr<int> p2 = std::move(p1); // ✅ ownership transferred

//    std::cout << *p2 << std::endl;
//     // now p1 is nullptr
// }


// int main() {
//     std::shared_ptr<int> p1 = std::make_shared<int>(20);
//     std::shared_ptr<int> p2 = p1;   // copy allowed!

//     std::cout << p1.use_count() << std::endl; // 2
// }


int main(){


    std::any a = 42;                    // holds an int
    std::any b = std::string("hello");  // holds a string
    std::any c = 3.14;                  // holds a double

    // To get the value back, you cast:
    std::string x = std::any_cast<std::string>(b);      // returns 42
    //std::string y=std::any_cast<int>(b);              // THROWS — b holds string, not int

    std::cout<<x<<"\n";
    return 0;
}

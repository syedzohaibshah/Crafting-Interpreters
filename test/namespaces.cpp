#include <iostream>

namespace Graphics {
    int x=10;
    class Shape {
    public:
        void draw() {
            std::cout << "Drawing shape\n";
        }
    };
}

namespace TextLib {
    class Text {
    public:
        void print() {
            std::cout << "Printing text\n";
        }
    };
}
//Global namespace
int x=5;
class Shape{

    public:
        void draw() {
            std::cout << "Drawing shape\n";
        }
};

int main() {
    Graphics::Shape s;
    TextLib::Text t;
    Shape S;
    s.draw();
    t.print();
    S.draw();


    std::cout<<"Graphics X value is "<<Graphics::x<<std::endl;
     std::cout<<"Global X value is "<<x<<std::endl;

}

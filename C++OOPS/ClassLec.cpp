#include <iostream>
#include<string>
using namespace std;
class house {
private:  // private access specifier
    // private member variables
    int length = 0; 
    int breadth = 0;
public:   // public access specifier
   void setdata(int l, int b) {
        length = l; // inside a class, we can access private members
        breadth = b;
    }
    void area() {
        cout << "Area of the house is: " << length * breadth << endl;
    }
};
int main() {
    house h;
    h.setdata(10, 20); // if it is not public, we cannot access it outside the class
    //h.length = 10; // This line will cause an error because length is private access pacifier
    h.area();
    return 0;
}



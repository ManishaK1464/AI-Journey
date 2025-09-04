#include <iostream>
#include<string>
using namespace std;
class Teacher {  // class definition
private:  // private access specifier
    // private member variables and properties
    double salary;
 
public:   // public access specifier
     string name;  // public member variable
     string dept;

     void setSalary(double s) { // set and get private member variable and methods
         salary = s;
     }
        double getSalary() { 
            return salary;
        }
}

int main() {
    Teacher t1;  // create object of class Teacher
    t1.name = "Alice";  // access public member variable
    t1.setSalary(50000);  // access public method to set private member variable
    cout << "Salary: $" << t1.getSalary() << endl;  // access public method to get private member variable
    return 0;
}

//encapuslation: bundling data and methods that operate on the data within one unit
// abstraction: hiding complex implementation details and showing only the essential features of the object
//inheritance: mechanism where a new class inherits properties and behavior from an existing class
//polymorphism: ability to present the same interface for different data types

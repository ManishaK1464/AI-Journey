#include <iostream>

using namespace std;

class Student
{
public:
    string name;
    int start_year;
    int end_year;

    // Constructor with member initializer list
    /*Student(string set_name, int set_start_year)  {
        name = set_name;
        start_year = set_start_year;*/
    
    Student(string name, int start_year) : 
      name(name),
      start_year(start_year) ,
      end_year ( start_year + 4 )
        {
            cout << "Constructor called for "  << endl;
        }// Assuming a 4-year program
    
};

int main() {
    // Create a Student object using the constructor
    Student s1("Alice", 2023);

    // Output the student's details
    cout << "Name: " << s1.name << endl
         << "Start Year: " << s1.start_year << endl
         << "End Year: " << s1.end_year << endl;

    return 0;
}

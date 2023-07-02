#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
using namespace std;

void print_cases(const std::string& name)
{
    string up ="";
    string lo = "";

    for(size_t i = 0; i < name.size(); i++)
    {
        up += std::toupper(name[i]);
        lo += std::tolower(name[i]);
    }

    std::cout << lo << " " << up << std::endl;
}

int main()
{
    print_cases("Elizabeth");
    print_cases("John");
    print_cases("Frank");
    print_cases("Fred");

    return 0;
}


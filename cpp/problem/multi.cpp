#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
using namespace std;

class ComplexNum
{
public:
    ComplexNum() : real(0), imaginary(0) {}
    ComplexNum(const string& input)
    {
        size_t idx = input.find("+");
        if (string::npos != idx)
        {
            real = stoi(input.substr(0, idx));
            imaginary = stoi(input.substr(idx + 1, input.size() - idx - 2));
        }
        else
        {
            if ( 'i' == input[input.size() - 1])
            {
                real = 0;
                imaginary = stoi(input.substr(0, input.size() - 1));
            }
            else
            {
                real = stoi(input);
                imaginary = 0;
            }
        }
    }

    ComplexNum multi(const ComplexNum& other)
    {
        ComplexNum ret;
        ret.real = real * other.real - imaginary * other.imaginary;
        ret.imaginary = real * other.imaginary + imaginary * other.real;
        return ret;
    }

    string serialize()
    {
        ostringstream stringStream;
        stringStream << real << "+" << imaginary << "i";
        return stringStream.str();
    }

private:
    int real;
    int imaginary;
};

int main()
{
    ComplexNum a("1+-1i");
    ComplexNum b("1+-1i");
    cout << a.multi(b).serialize() << endl;
    return 0;
}
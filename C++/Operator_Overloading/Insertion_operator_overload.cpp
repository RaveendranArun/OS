#include <iostream>

using namespace std;
class Complex 
{
    int real;
    int img;
public:
    Complex(int a = 0, int b= 0)
    {
        real = a;
        img = b;
    }

    friend ostream & operator<<(ostream& o, Complex c);
};

ostream & operator<<(ostream& o, Complex c)
{
    o << c.real << "+i" << c.img;
    return o;
}

int main()
{
    Complex c1(2, 3), c2(5, 2);
    operator<<(operator<<(cout, c1) << endl, c2) << endl;;
    return 0;
}

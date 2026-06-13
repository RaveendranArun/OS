#include <iostream>

class Counter
{
    int value;
public:
    Counter(int v = 0)
    {
        value = v;
    }
    
    Counter& operator++()
    {
        ++value;
        return *this;
    }

    Counter operator++(int)
    {
        Counter temp = *this;
        value++;
        return temp;
    }

    friend std::ostream& operator<<(std::ostream& out, const Counter& c);
   
};

std::ostream& operator<<(std::ostream& out, const Counter& c)
{
    out << c.value;
    return out;
}

int main()
{
    Counter c(5);
    std::cout << "Pre: " << ++c << std::endl;
    std::cout << "After: " << c << std::endl;
     
    std::cout << "Post: " << c++ << std::endl;
    std::cout << "After: " << c << std::endl;
    return 0; 
}
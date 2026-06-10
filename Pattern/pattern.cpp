#include <iostream>

void pattern(int n)
{
    for (int i = 1; i <= n; ++i)
    {
        int num = i;
        int diff = n-1;
        for (int j = 1; j <= i; ++j)
        {
            std::cout << num << " ";
            num += diff;
        }
        std::cout << std::endl;
    }
}

int main()
{
    pattern(6);
    return 0;
}
#include <iostream>
#include <string>
#include <vector>

int main()
{
    while (true)
    {
        std::cout << "myshell$ " << std::flush;
        std::string input{};

        if (!std::getline(std::cin, input))
        {
            std::cout << '\n';
            break;
        }
    }
    return 0;
}

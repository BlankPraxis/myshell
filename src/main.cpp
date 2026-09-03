#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

std::vector<std::string> parse(std::string& str, char delimeter=' ')
{
    std::vector<std::string> tokens{};
    
    size_t i { 0 };
    for (size_t j{}; j < str.length(); ++j)
    {
        if (str[j] == delimeter)
        {
            if (str[i] == delimeter)
            {
                i = j;
                continue;
            }

            tokens.push_back(str.substr(i, j - i));
            i = j;
        }
        else
        {
            if (str[i] == delimeter)
            {
                i = j;
                continue;
            }
        }
        
    }
    std::string end { str.substr(i) };
    if (!end.empty() && end.find(delimeter) == std::string::npos)
    {
        tokens.push_back(end);
    }
    return tokens;
}

void exit(std::vector<std::string>& args)
{
    std::exit(0);
}

void echo(std::vector<std::string>& args)
{
    for (const auto& element: args)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}

int main()
{
    while (true)
    {
        std::cout << "$ " << std::flush;
        std::string input{};

        if (!std::getline(std::cin, input))
        {
            std::cout << '\n';
            break;
        }

        std::vector<std::string> parsed_command { parse(input) };
        std::string command_name { parsed_command[0] };
        std::vector<std::string> command_args(parsed_command.begin() + 1, parsed_command.end());

        if (command_name == "exit")
        {
            exit(command_args);
        }
        else if (command_name == "echo")
        {
            echo(command_args);
        }
    }
    return 0;
}

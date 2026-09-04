#include "parser.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace shell
{
    std::vector<std::string> parse_command(const std::string& str, char delimeter)
    {
        std::vector<std::string> tokens{};

        std::size_t i{};
        for (std::size_t j{}; j < str.length(); ++j)
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

        std::cout << '[' << tokens[0];
        for (size_t i { 1 }; i < tokens.size(); ++i)
        {
            std::cout << ", " << tokens[i];
        }
        std::cout << "]\n";

        return tokens;
    }

    std::vector<std::string> parse_path(const std::string& str)
    {
        std::vector<std::string> dirs{};
        return dirs;
    }
}


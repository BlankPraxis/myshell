#include "parser.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace shell
{
    std::vector<std::string> parse(std::string& str, char delimeter)
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

        return tokens;
    }
}


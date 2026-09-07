#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#include "status.hpp"
#include "service.hpp"
#include "parser.hpp"
#include "colors.hpp"

namespace fs = std::filesystem;

void debug(const char *test_string, std::vector<std::string> test_vect, int test_num)
{
    std::cout << "test " << test_num << ": " << test_string << '\n';
    if (test_vect.empty())
    {
        std::cout << "expected [\"\"]\n";
    }
    std::vector<std::string> output { shell::parse_path(test_string) };
    if (output.empty())
    {
        std::cout << "actual   [\"\"]\n";
        std::cout << "passed\n";
        return;
    }

    std::cout << "expected [" << '\"' << test_vect[0] << '\"';
    for (size_t i { 1 }; i < test_vect.size(); ++i)
    {
        std::cout << ", " << '\"' << test_vect[i] << '\"'; 
    }
    std::cout << "]\n";


    std::cout << "actual   [" << '\"' << output[0] << '\"';
    for (size_t i { 1 }; i < output.size(); ++i)
    {
        std::cout << ", " << '\"' << output[i] << '\"'; 
    }
    std::cout << "]\n";

    if (test_vect == shell::parse_path(test_string))
    {
        std::cout << "passed\n";
    }
    else
    {
        std::cout << "failed\n";
    }
}

int main()
{
    while (true)
    {   
        std::string path { fs::current_path().string() };
        
        const char *home_env { std::getenv("HOME") };
        if (home_env)
        {
            std::string home { home_env };
            path.replace(0, home.length(), "~");
        }

        std::cout << color::blue << path << color::reset << "$ " << std::flush;
        std::string input{};

        if (!std::getline(std::cin, input))
        {
            std::cout << '\n';
            break;
        }

        std::vector<std::string> parsed_command { shell::parse_command(input) };
        if (parsed_command.empty())
        {
            continue;
        }
        // std::string command_name { parsed_command[0] };
        // std::vector<std::string> command_args(parsed_command.begin() + 1, parsed_command.end());

        // shell::Status state { shell::direct_to_command(command_name, command_args) };
        
        // if (state == shell::Status::EXIT) return EXIT_SUCCESS;
        // if (state == shell::Status::ERROR) return EXIT_FAILURE;
    }
    return 0;
}

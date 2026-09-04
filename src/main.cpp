#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#include "status.hpp"
#include "service.hpp"
#include "parser.hpp"

int main()
{
    while (true)
    {   
        std::filesystem::path path { std::filesystem::current_path() };
        std::cout << path.string() << "$ " << std::flush;
        std::string input{};

        if (!std::getline(std::cin, input))
        {
            std::cout << '\n';
            break;
        }

        std::vector<std::string> parsed_command { shell::parse(input) };
        std::string command_name { parsed_command[0] };
        std::vector<std::string> command_args(parsed_command.begin() + 1, parsed_command.end());

        shell::Status state { shell::direct_to_command(command_name, command_args) };
        
        if (state == shell::Status::EXIT) return EXIT_SUCCESS;
        if (state == shell::Status::ERROR) return EXIT_FAILURE;
    }
    return 0;
}

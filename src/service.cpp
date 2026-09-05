#include "service.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "builtins.hpp"
#include "status.hpp"
#include "executables.hpp"

namespace shell
{
    shell::Status direct_to_command(const std::string& command_name, std::vector<std::string>& command_args)
    {
        std::vector<std::string> builtins { shell::builtin_commands() };
        if (std::find(builtins.begin(), builtins.end(), command_name) != builtins.end())
        {
            if (command_name == "exit")
            {
                return shell::exit(command_args);
            }
            if (command_name == "echo")
            {
                return shell::echo(command_args);
            }
            if (command_name == "pwd")
            {
                return shell::pwd();
            }
            if (command_name == "cd")
            {
                return shell::cd(command_args);
            }
            if (command_name == "type")
            {
                return shell::type(command_args);
            }
        }
        else
        {
            auto executable { shell::find_executable(command_name) };
            if (executable)
            {
                return shell::excecute(command_name, command_args, executable.value());
            }
        }

        std::cout << command_name << ": command not found\n";
        return shell::Status::CONTINUE;
    }
}

#pragma once

#include <string>
#include <vector>

#include "status.hpp"

namespace shell
{
    std::vector<std::string> builtin_commands();
    shell::Status exit(std::vector<std::string>& args);
    shell::Status echo(std::vector<std::string>& args);
    shell::Status pwd();
    shell::Status cd(std::vector<std::string>& args);
    shell::Status type(std::vector<std::string>& args);
} 

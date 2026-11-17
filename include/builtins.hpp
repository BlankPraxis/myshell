#pragma once

#include <string>
#include <vector>

#include "status.hpp"

namespace shell
{
    std::vector<std::string> builtin_commands();
    Status exit(const std::vector<std::string>& args);
    Status echo(const std::vector<std::string>& args);
    Status pwd();
    Status cd(const std::vector<std::string>& args);
    Status type(const std::vector<std::string>& args);
}

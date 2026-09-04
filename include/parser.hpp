#pragma once

#include <string>
#include <vector>

namespace shell
{
    std::vector<std::string> parse_command(const std::string& str, char delimeter=' ');
    std::vector<std::string> parse_path(const std::string& str);
}

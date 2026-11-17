#pragma once

#include <string>
#include <vector>

#include "command.hpp"

namespace shell
{
    // returns an empty pipeline for blank input. Throws std::invalid_argument
    // for malformed input (such as a missing redirection target).
    Pipeline parse_command(const std::string& str);
    std::vector<std::string> parse_path(const std::string& str);
}

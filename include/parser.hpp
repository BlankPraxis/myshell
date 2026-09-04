#pragma once

#include <string>
#include <vector>

namespace shell
{
    std::vector<std::string> parse(std::string& str, char delimeter=' ');
}

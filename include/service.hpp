#include <string>
#include <vector>

#include "status.hpp"

namespace shell
{
    shell::Status direct_to_command(const std::string& command_name, std::vector<std::string>& command_args);
} 

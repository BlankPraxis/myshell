#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "command.hpp"

namespace shell
{
    std::optional<std::filesystem::path> find_executable(const std::string& command_to_find);

    // Called only from a forked child. Replaces the child with an executable;
    // reports an error and exits the child if the executable cannot start.
    [[noreturn]] void execute_external(const Command& command);
}

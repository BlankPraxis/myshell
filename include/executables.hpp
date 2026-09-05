#include <optional>
#include <filesystem>
#include <string>
#include <vector>

#include "status.hpp"

namespace fs = std::filesystem;

namespace shell
{
    std::optional<fs::path> find_executable(const std::string& command_to_find);
    shell::Status excecute(const std::string& command_name, std::vector<std::string>& command_args, fs::path executable);
}

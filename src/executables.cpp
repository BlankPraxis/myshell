#include "executables.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <unistd.h>
#include <vector>

#include "parser.hpp"

namespace fs = std::filesystem;

namespace shell
{
    std::optional<fs::path> find_executable(const std::string& command_to_find)
    {
        if (command_to_find.empty()) return std::nullopt;

        auto is_executable = [](const fs::path& candidate) {
            std::error_code ec;
            return fs::is_regular_file(candidate, ec) && !ec &&
                   access(candidate.c_str(), X_OK) == 0;
        };

        // Explicit paths do not require PATH to exist.
        if (command_to_find.find('/') != std::string::npos)
        {
            const fs::path candidate = command_to_find;
            if (is_executable(candidate)) return candidate;
            return std::nullopt;
        }

        const char* path_env = std::getenv("PATH");
        if (!path_env) return std::nullopt;
        for (const std::string& dir : parse_path(path_env))
        {
            // Empty PATH entries represent the current directory.
            const fs::path candidate = fs::path(dir.empty() ? "." : dir) / command_to_find;
            if (is_executable(candidate)) return candidate;
        }
        return std::nullopt;
    }

    [[noreturn]] void execute_external(const Command& command)
    {
        const auto executable = find_executable(command.name);
        if (!executable)
        {
            std::cerr << command.name << ": command not found\n" << std::flush;
            _exit(127);
        }

        // argv points at strings owned by Command, which stay alive until execv.
        std::vector<char*> argv;
        argv.reserve(command.args.size() + 2);
        argv.push_back(const_cast<char*>(command.name.c_str()));
        for (const std::string& arg : command.args)
            argv.push_back(const_cast<char*>(arg.c_str()));
        argv.push_back(nullptr);

        execv(executable->c_str(), argv.data());
        const int error = errno;
        std::perror(("shell: " + command.name).c_str());
        _exit(error == ENOENT ? 127 : 126);
    }
}

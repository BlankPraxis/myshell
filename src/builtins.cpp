#include "builtins.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#include "executables.hpp"

namespace fs = std::filesystem;

namespace shell
{
    std::vector<std::string> builtin_commands()
    {
        return {"exit", "echo", "pwd", "cd", "type"};
    }

    Status exit(const std::vector<std::string>& args)
    {
        if (args.empty() || args[0] == "0") return Status::EXIT;
        return Status::ERROR;
    }

    Status echo(const std::vector<std::string>& args)
    {
        for (std::size_t i = 0; i < args.size(); ++i)
        {
            if (i) std::cout << ' ';
            std::cout << args[i];
        }
        std::cout << '\n';
        return Status::CONTINUE;
    }

    Status pwd()
    {
        std::cout << fs::current_path().string() << '\n';
        return Status::CONTINUE;
    }

    Status cd(const std::vector<std::string>& args)
    {
        std::string path;
        if (args.empty() || args[0] == "~" || args[0].rfind("~/", 0) == 0)
        {
            const char* home = std::getenv("HOME");
            if (!home)
            {
                std::cerr << "shell: cd: HOME not set\n";
                return Status::CONTINUE;
            }
            path = home;
            if (!args.empty() && args[0].size() > 1) path += args[0].substr(1);
        }
        else path = args[0];

        std::error_code ec;
        fs::current_path(fs::path(path), ec);
        if (ec) std::cerr << "shell: cd: " << path << ": " << ec.message() << '\n';
        return Status::CONTINUE;
    }

    Status type(const std::vector<std::string>& args)
    {
        if (args.empty()) return Status::CONTINUE;
        const std::vector<std::string> builtins = builtin_commands();
        if (std::find(builtins.begin(), builtins.end(), args[0]) != builtins.end())
        {
            std::cout << args[0] << " is a shell builtin\n";
        }
        else if (auto executable = find_executable(args[0]))
        {
            std::cout << args[0] << " is " << executable->string() << '\n';
        }
        else std::cout << "shell: type: " << args[0] << ": not found\n";
        return Status::CONTINUE;
    }
}

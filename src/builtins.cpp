#include "builtins.hpp"

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#include "status.hpp"

namespace fs = std::filesystem;

namespace shell
{
    std::vector<std::string> builtin_commands()
    {
        return std::vector<std::string> {
            "exit",
            "echo",
            "pwd",
            "cd",
            "type"
        };
    }

    shell::Status exit(std::vector<std::string>& args)
    {
        if (args.empty() || args[0] == "0")
        {
            return shell::Status::EXIT;
        }
        return shell::Status::ERROR;
    }

    shell::Status echo(std::vector<std::string>& args)
    {
        if (args.empty()) 
        {
            std::cout << '\n';
            return shell::Status::CONTINUE;
        }
        
        std::cout << args[0];
        for (std::size_t i { 1 }; i < args.size(); ++i)
        {
            std::cout << ' ' << args[i];
        }
        std::cout << std::endl;
        return shell::Status::CONTINUE;
    }

    shell::Status pwd()
    {
        std::cout << fs::current_path().string() << '\n';
        return shell::Status::CONTINUE;
    }

    shell::Status cd(std::vector<std::string>& args)
    {
        std::error_code ec;
        std::string path {};
        if (args.empty())
        {
            path = "/home/whoischristian/";
        }
        else if (args[0] == "~")
        {
            path = "/home/whoischristian/";
        }
        if (path.empty())
        {
            fs::path new_dir { args[0] };
            fs::current_path(new_dir, ec);
            if (ec)
            {
                std::cerr << "shell: cd: " << args[0] << ": No such file or directory\n";
            }
            return shell::Status::CONTINUE;
        }
        fs::path new_dir { path };
        fs::current_path(new_dir, ec);
        if (ec)
        {
            std::cerr << "shell: cd: " << args[0] << ": No such file or directory\n";
        }
        return shell::Status::CONTINUE;
    }

    shell::Status type(std::vector<std::string>& args)
    {
        if (args.empty()) return shell::Status::CONTINUE;

        std::vector<std::string> builtins { builtin_commands() };
        if (std::find(builtins.begin(), builtins.end(), args[0]) != builtins.end())
        {
            std::cout << args[0] << " is a shell builtin\n";
            return shell::Status::CONTINUE; 
        }
        else
        {
            // find eternal command
        }

        std::cout << "shell: type: " << args[0] << ": not found\n";
        return shell::Status::CONTINUE;
    }
}
